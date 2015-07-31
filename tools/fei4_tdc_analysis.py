"""Analyze the tdc data from simulation. Usefull for tdc data comparison with real measurement.
"""
import tables as tb
import numpy as np
import progressbar
import os
import math
import sys
import glob

from pybar.analysis.analyze_raw_data import AnalyzeRawData
import pybar.scans.analyze_source_scan_tdc_data as tdc_analysis

def analyze_hits(input_file_hits):
    with AnalyzeRawData(raw_data_file=None, analyzed_data_file=input_file_hits) as analyze_raw_data:
        analyze_raw_data.create_source_scan_hist = True
        analyze_raw_data.create_cluster_hit_table = True
        analyze_raw_data.create_cluster_table = True
        analyze_raw_data.create_cluster_size_hist = True
        analyze_raw_data.create_cluster_tot_hist = True
        analyze_raw_data.create_tdc_hist = True
        analyze_raw_data.analyze_hit_table(analyzed_data_out_file=input_file_hits[:-3] + '_analyzed.h5')
        analyze_raw_data.plot_histograms(pdf_filename=input_file_hits[:-3], analyzed_data_file=input_file_hits[:-3] + '_analyzed.h5')

def analyze_tdc(hit_file, calibration_filename=None, col_span=[5, 75], row_span=[10, 320]):
    # Data files
    hit_cut_file = hit_file[:-3] + '_cut_hits.h5'
    hit_cut_analyzed_file = hit_file[:-3] + '_cut_hits_analyzed.h5'
    # Selection criterions
    hit_selection = '(column > %d) & (column < %d) & (row > %d) & (row < %d)' % (col_span[0] + 1, col_span[1] - 1, row_span[0] + 5, row_span[1] - 5)  # deselect edge pixels for better cluster size cut
    hit_selection_conditions = ['(n_cluster==1)', '(n_cluster==1) & (cluster_size == 1)', '(n_cluster==1) & (cluster_size == 1) & ((tot > 12) | ((TDC * 1.5625 - tot * 25 < 100) & (tot * 25 - TDC * 1.5625 < 100))) & %s' % hit_selection]
    event_status_select_mask = 0b0000111111111111
    event_status_condition = 0b0000000100000000  # trigger, tdc word and perfect event structure required

    tdc_analysis.histogram_tdc_hits(hit_file, hit_selection_conditions, event_status_select_mask, event_status_condition, calibration_filename, max_tdc=1500, n_bins=1000)


if __name__ == "__main__":
    arguments = sys.argv
    if len(arguments) < 2:
        print 'Please provide the base file name of the root data files (e.g. threshold_ for threshold_2000.root)'
        raise SystemExit
     
    base_file_name = arguments[1]

    calibration_filename = r'/home/davidlp/geant4/SourceSim-build/converter/calibration_data/hit_or_calibration_calibration.h5'
    file_names = glob.glob(base_file_name + '*_interpreted.h5')
    file_names.sort()
   
    for file_name in file_names:
        analyze_hits(file_name)
    
    file_names = glob.glob(base_file_name + '*_interpreted_analyzed.h5')
    file_names.sort()
 
    for file_name in file_names:
        analyze_tdc(file_name, calibration_filename)