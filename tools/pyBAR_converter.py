"""This script converts a hdf5 table into a CERN ROOT Ttree.
"""
import tables as tb
import numpy as np
import ctypes
import progressbar
import os
import math
import sys
import glob
import re

from multiprocessing import Pool

import ROOT as r
from pybar.fei4 import register_utils
from pybar.analysis.RawDataConverter import data_struct


def get_charge_calibration(tdc_calibation_file, plsr_dac_calibation_file):
    with tb.openFile(tdc_calibation_file, mode="r") as in_file_calibration_h5:
        tdc_calibration, tdc_error = in_file_calibration_h5.root.HitOrCalibration[:, :, :, 1], in_file_calibration_h5.root.HitOrCalibration[:, :, :, 3]
        tot_calibration, tot_error = in_file_calibration_h5.root.HitOrCalibration[:, :, :, 0], in_file_calibration_h5.root.HitOrCalibration[:, :, :, 2]
        tdc_calibration_values = in_file_calibration_h5.root.HitOrCalibration.attrs.scan_parameter_values[:]
        global_config = register_utils.parse_global_config(plsr_dac_calibation_file)
        c_high, vcal_c0, vcal_c1 = global_config['C_Inj_High'], global_config['Vcal_Coeff_0'], global_config['Vcal_Coeff_1']
        charge_calibration_values = (vcal_c0 + vcal_c1 * tdc_calibration_values) * c_high / 0.16022

    return charge_calibration_values, tdc_calibration, tdc_error, tot_calibration, tot_error


def create_hit_table(input_file_name, tdc_calibation_file, plsr_dac_calibation_file, n_sub_files=8):  # loops over all root files and merges the data into a hdf5 file aligned at the event number
    print 'Converting data from CERN ROOT TTree to hdf5 table'
    charge_calibration_values, tdc_calibration, tdc_error, tot_calibration, tot_error = get_charge_calibration(tdc_calibation_file, plsr_dac_calibation_file)

    # add all files that have the input_file_name praefix and load their data
    input_file_names = [input_file_name + '_t%d.root' % index for index in range(n_sub_files) if os.path.isfile(input_file_name + '_t%d.root' % index)]
    n_files = len(input_file_names)
    input_files_root = [r.TFile(file_name, 'read') for file_name in input_file_names]
    pixel_digits = [input_file_root.Get('EventData').Get('Pixel Digits') for input_file_root in input_files_root]
    n_hits = [pixel_digit.GetEntries() for pixel_digit in pixel_digits]  # total pixel hits to analyze
    n_total_hits = sum(n_hits)

    with tb.open_file(input_file_name + '_interpreted.h5', 'w') as out_file_h5:
        hit_table = out_file_h5.create_table(out_file_h5.root, name='Hits_0', description=data_struct.HitInfoTable, title='hit_data', filters=tb.Filters(complib='blosc', complevel=5, fletcher32=False))

        # tmp data structures to be filles by ROOT
        data = {}
        for index, pixel_digit in enumerate(pixel_digits):
            column_data = {}
            for branch in pixel_digit.GetListOfBranches():  # loop over the branches
                column_data[branch.GetName()] = np.zeros(shape=1, dtype=np.int32)
                branch.SetAddress(column_data[branch.GetName()].data)
            data[index] = column_data

        # result data structur to be filles in the following loop
        hits = np.zeros((n_total_hits,), dtype=tb.dtype_from_descr(data_struct.HitInfoTable))

        # get file index with lowest event number
        for pixel_digit in pixel_digits:
            pixel_digit.GetEntry(0)
        min_event_number = min([data[index]['event'][0] for index in range(n_files)])
        actual_file_index = np.where(np.array([data[index]['event'][0] for index in range(n_files)]) == min_event_number)[0][0]

        indices = [0] * n_files

        table_index = 0

        actual_data = data[actual_file_index]
        actual_event_number = actual_data['event'][0]
        last_valid_event_number = 0
        last_tdc = 0
        expected_event_number = actual_event_number
        indices[actual_file_index] = 1

        progress_bar = progressbar.ProgressBar(widgets=['', progressbar.Percentage(), ' ', progressbar.Bar(marker='*', left='|', right='|'), ' ', progressbar.AdaptiveETA()], maxval=n_total_hits, term_width=80)
        progress_bar.start()

        def add_actual_data(actual_data, table_index, hits):
            if actual_data['column'] >= col_span[0] and actual_data['column'] < col_span[1] and actual_data['row'] >= row_span[0] and actual_data['row'] < row_span[1]:  # sanity check
                tdc_interpolation = interp1d(x=charge_calibration_values, y=tdc_calibration[actual_data['column'], actual_data['row']], kind='slinear', bounds_error=False, fill_value=-1)
                tdc_error_interpolation = interp1d(x=charge_calibration_values, y=tdc_error[actual_data['column'], actual_data['row']], kind='slinear', bounds_error=False, fill_value=1)
                tdc = tdc_interpolation(actual_data['charge'])[0][0]
#                 if tdc >= 0:
#                     tdc_e = np.abs(tdc_error_interpolation(actual_data['charge'])[0][0])
#                     if tdc_e > 0:
#                         tdc = np.random.normal(tdc, tdc_e, size=1)[0]  # add noise to it from TDC method
                    
                tot_interpolation = interp1d(x=charge_calibration_values, y=tot_calibration[actual_data['column'], actual_data['row']], kind='slinear', bounds_error=False, fill_value=-1)
                tot = tot_interpolation(actual_data['charge'])[0][0]

                if math.isnan(tdc):  # do not add hits where tdc is nan, these pixel have a very high threshold or do not work
                    return table_index

                if tdc == -1:
                    if actual_data['charge'] > 5000:  # no calibration for TDC due to high charge, thus mark as TDC overflow event
                        hits[table_index]['event_status'] |= 0b0000010100000000
                        tdc = 4095
                    else:  # below threshold thus no calibration, do not add hit
                        return table_index

                if tot == -1:
                    if actual_data['charge'] > 5000:  # most likely no calibration for TOT due to high charge, thus set max tot
                        tot = 13
                    else:  # below threshold thus no calibration, do not add hit
                        print 'WARNING: Should never trigger!'
                        return table_index

                hits[table_index]['event_status'] |= 0b0000000100000000  # set TDC and trigger word
                hits[table_index]['event_number'] = actual_data['event'][0].astype(np.int64)
                hits[table_index]['column'] = (actual_data['column'] + 1).astype(np.uint8)
                hits[table_index]['row'] = (actual_data['row'] + 1).astype(np.uint16)
                hits[table_index]['TDC'] = int(round(tdc, 0))
                hits[table_index]['tot'] = int(round(tot, 0))

                table_index += 1
            return table_index           

        while True:
            actual_event_number = actual_data['event'][0]
            if (actual_event_number == expected_event_number or actual_event_number == expected_event_number - 1):  # check if event number increases
                actual_index, actual_digits, actual_data = indices[actual_file_index], pixel_digits[actual_file_index], data[actual_file_index]
                table_index = add_actual_data(actual_data, table_index, hits)
            else:  # event number does not increase, thus the events are maybe in another file --> switch file or the event number is missing
                file_event_numbers = [data[file_index]['event'][0] for file_index in range(n_files)]  # all files actual event number
                actual_file_index = np.where(file_event_numbers == min(file_event_numbers))[0][0]
                actual_index, actual_digits, actual_data = indices[actual_file_index], pixel_digits[actual_file_index], data[actual_file_index]
                actual_event_number = actual_data['event'][0]
                table_index = add_actual_data(actual_data, table_index, hits)

            progress_bar.update(table_index)
            expected_event_number = actual_event_number + 1  # next expected event number does not have to exists, simulated events without hits are possible
            actual_digits.GetEntry(actual_index)

            if indices[actual_file_index] < n_hits[actual_file_index]:  # simply stop when the first file is fully iterated, some data is lost of cause
                indices[actual_file_index] += 1
            else:
                break

        # Set missing data and store to file
        hits[:table_index]['LVL1ID'] = hits[:table_index]['event_number'] % 255
        hits[:table_index]['BCID'] = hits[:table_index]['LVL1ID']
        hits[:table_index]['relative_BCID'] = 6
        hit_table.append(hits[:table_index])

        progress_bar.finish()

        for input_file_root in input_files_root:
            input_file_root.Close()
            
        del hits
            
    # Set data that is common to all hits of one event
    with tb.open_file(input_file_name + '_interpreted.h5', 'a') as out_file_h5:
        hits = out_file_h5.root.Hits_0[:]
        progress_bar = progressbar.ProgressBar(widgets=['', progressbar.Percentage(), ' ', progressbar.Bar(marker='*', left='|', right='|'), ' ', progressbar.AdaptiveETA()], maxval=hits.shape[0], term_width=80)
        progress_bar.start()
        actual_event_number = -1
        for ihit in range(hits.shape[0]):
            if hits[ihit]['event_number'] != actual_event_number:  # Detect new event
                if actual_event_number != -1:
                    for jhit in xrange(ihit - 1, -1, -1):
                        if hits[jhit]['event_number'] != actual_event_number:
                            break
                        hits[jhit]['TDC'] = max_tdc
                        hits[jhit]['event_status'] = max_event_status
                actual_event_number, max_tdc, max_event_status = hits[ihit]['event_number'], hits[ihit]['TDC'], hits[ihit]['event_status']
            else:
                max_event_status |= hits[ihit]['event_status']
                if max_tdc < hits[ihit]['TDC']:
                    max_tdc = hits[ihit]['TDC']
            progress_bar.update(ihit)
#             out_file_h5.remove_node(out_file_h5.root.Hits)
        hit_table = out_file_h5.create_table(out_file_h5.root, name='Hits', description=data_struct.HitInfoTable, title='hit_data', filters=tb.Filters(complib='blosc', complevel=5, fletcher32=False))
        hit_table.append(hits)
        progress_bar.finish()

if __name__ == "__main__":
    import matplotlib.pyplot as plt
    from scipy.interpolate import interp1d
    import fei4_tdc_analysis as tdcana

    plsr_dac_calibation_file = r'/home/davidlp/git/SourceSim/tools/calibration_data/plsr_dac_scan.cfg'
    tdc_calibation_file = r'/home/davidlp/git/SourceSim/tools/calibration_data/hit_or_calibration_calibration.h5'
    
    col_span = [10, 70]
    row_span = [10, 320]
 
#     arguments = sys.argv
#     if len(arguments) < 2:
#         print 'Please provide the base file name of the root data files'
#         raise SystemExit
     
    base_file_name = '/media/davidlp/Data/simulation/'#arguments[1]
     
    file_names = glob.glob(base_file_name + '*_t*.root')
    file_names.sort()
     
    input_file_names = {}
     
    for file_name in file_names:
        parameter = int(re.findall( r'_?[0-9]+_', file_name)[0][1:-1])
        input_file_names[parameter] = file_name[:file_name.rfind('_')]
        
    def _function_wrapper_create_hit_table(input_file_name):  # needed for multiprocessing call with arguments
        return create_hit_table(input_file_name, tdc_calibation_file, plsr_dac_calibation_file)
     
#     # Multiprocessing for serveral runs
#     pool = Pool()  # let all cores work the array
#     pool.map(_function_wrapper_create_hit_table, input_file_names.values())
#     pool.close()
#     pool.join()
    
    #create_hit_table(input_file_names.values()[0], tdc_calibation_file, plsr_dac_calibation_file)
    tdcana.analyze_hits(r'/media/davidlp/Data/simulation/threshold_4000_interpreted.h5')


#     with tb.openFile(tdc_calibation_file, mode="r") as in_file_calibration_h5:
#         tdc_calibration, tdc_error = in_file_calibration_h5.root.HitOrCalibration[:, :, :, 1], in_file_calibration_h5.root.HitOrCalibration[:, :, :, 3]
#         tot_calibration, tot_error = in_file_calibration_h5.root.HitOrCalibration[:, :, :, 0], in_file_calibration_h5.root.HitOrCalibration[:, :, :, 2]
#         tdc_calibration_values = in_file_calibration_h5.root.HitOrCalibration.attrs.scan_parameter_values[:]
#         global_config = register_utils.parse_global_config(plsr_dac_calibation_file)
#         c_high, vcal_c0, vcal_c1 = global_config['C_Inj_High'], global_config['Vcal_Coeff_0'], global_config['Vcal_Coeff_1']
#         charge_calibration_values = (vcal_c0 + vcal_c1 * tdc_calibration_values) * c_high / 0.16022
# 
#         column, row = 20, 30
#         print charge_calibration_values, tdc_calibration[column, row]
#         tdc_interpolation = interp1d(x=charge_calibration_values, y=tdc_calibration[column, row], kind='slinear', bounds_error=False, fill_value=0)
#         tdc_error_interpolation = interp1d(x=charge_calibration_values, y=tdc_error[column, row], kind='slinear', bounds_error=False, fill_value=0)
#         tot_interpolation = interp1d(x=charge_calibration_values, y=tot_calibration[column, row], kind='slinear', bounds_error=False, fill_value=0)
#         tot_error_interpolation = interp1d(x=charge_calibration_values, y=tot_error[column, row], kind='slinear', bounds_error=False, fill_value=0)

#         plt.plot(np.arange(2000, 10000, 1), tdc_interpolation(np.arange(2000, 10000, 1)), '-')
#         plt.plot(np.arange(2000, 10000, 1), tdc_error_interpolation(np.arange(2000, 10000, 1)), '-')
#         plt.plot(np.arange(2000, 10000, 1), tot_interpolation(np.arange(2000, 10000, 1)), '-')
#         plt.plot(np.arange(2000, 10000, 1), tot_error_interpolation(np.arange(2000, 10000, 1)), '-')
#         plt.show()
