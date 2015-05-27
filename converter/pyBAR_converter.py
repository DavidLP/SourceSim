"""This script converts a hdf5 table into a CERN ROOT Ttree.
"""
import tables as tb
import numpy as np
import ctypes
import progressbar
import os

import ROOT as r
from pybar.analysis.RawDataConverter import data_struct


def create_hit_table(input_file_name, n_sub_files=8):  # loops over all root files and merges the data into a hdf5 file aligned at the event number
    # add all files that have the input_file_name praefix and load their data
    input_file_names = [input_file_name + '_t%d.root' % index for index in range(n_sub_files) if os.path.isfile(input_file_name + '_t%d.root' % index)]
    n_files = len(input_file_names)
    input_files_root = [r.TFile(file_name, 'read') for file_name in input_file_names]
    pixel_digits = [input_file_root.Get('EventData').Get('Pixel Digits') for input_file_root in input_files_root]
    n_hits = [pixel_digit.GetEntries() for pixel_digit in pixel_digits]  # total pixel hits to analyze
    n_total_hits = sum(n_hits)

    with tb.open_file(input_file_name + '_interpreted.h5', 'w') as out_file_h5:
        hit_table = out_file_h5.create_table(out_file_h5.root, name='Hits', description=data_struct.HitInfoTable, title='hit_data', filters=tb.Filters(complib='blosc', complevel=5, fletcher32=False))

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
        expected_event_number = actual_event_number
        indices[actual_file_index] = 1

        progress_bar = progressbar.ProgressBar(widgets=['', progressbar.Percentage(), ' ', progressbar.Bar(marker='*', left='|', right='|'), ' ', progressbar.AdaptiveETA()], maxval=n_total_hits, term_width=80)
        progress_bar.start()
        
        def add_actual_data(actual_data, table_index):
            if actual_data['column'] >= 0 and actual_data['column'] < 80 and actual_data['row'] >= 0 and actual_data['row'] < 336:
                hits[table_index]['event_number'] = actual_data['event'][0].astype(np.int64)
                hits[table_index]['column'] = (actual_data['column'] + 1).astype(np.uint8)
                hits[table_index]['row'] = (actual_data['row'] + 1).astype(np.uint16)
                hits[table_index]['tot'] = 1
                hits[table_index]['LVL1ID'] = hits[table_index]['event_number'] % 255
                hits[table_index]['BCID'] = 6

        while True:
            actual_event_number = actual_data['event'][0]
            if (actual_event_number == expected_event_number or actual_event_number == expected_event_number - 1):  # check if event number increases
                actual_index, actual_digits, actual_data = indices[actual_file_index], pixel_digits[actual_file_index], data[actual_file_index]
                add_actual_data(actual_data, table_index)
            else:  # event number does not increase, thus the events are in another file --> switch file or the event number is missing
                file_event_numbers = [data[file_index]['event'][0] for file_index in range(n_files)]  # all files actual event number
                actual_file_index = np.where(file_event_numbers == min(file_event_numbers))[0][0]
                actual_index, actual_digits, actual_data = indices[actual_file_index], pixel_digits[actual_file_index], data[actual_file_index]
                actual_event_number = actual_data['event'][0]
                add_actual_data(actual_data, table_index)

            progress_bar.update(table_index)
            table_index += 1
            expected_event_number = actual_event_number + 1
            actual_digits.GetEntry(actual_index)

            if indices[actual_file_index] < n_hits[actual_file_index]:  # simply stop when the first file is fully iterated
                indices[actual_file_index] += 1
            else:
                break

        hit_table.append(hits[:table_index])

        progress_bar.finish()

        for input_file_root in input_files_root:
            input_file_root.Close()


if __name__ == "__main__":
    create_hit_table(r'/home/davidlp/geant4/SourceSim-build/SourceSimulation')
