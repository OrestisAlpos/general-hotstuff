import sys
import argparse
import numpy as np
from collections import defaultdict
import matplotlib.pyplot as plt

from plot_settings import *

def plot_n():
    plt.grid(True)
    plt.xlabel("Throughput (Kops/sec)")
    plt.ylabel("Latency (msec)")
    x_Axis = list(threshold_tot.keys())
    y_Axis_Thresh = list(threshold_tot.values())
    line_Thresh = plt.plot(x_Axis, y_Axis_Thresh, label=offLabel, color=offColor, marker='*',  markersize=7) 
    
    x_Axis = list(form_tot.keys())
    y_Axis_Form = list(form_tot.values())
    line_Form = plt.plot(x_Axis, y_Axis_Form, label=formLabel, color=formColor, marker='1')

    x_Axis = list(generalized_tot.keys())
    y_Axis_Gen = list(generalized_tot.values())
    line_Gen = plt.plot(x_Axis, y_Axis_Gen, label=onLabel, color=onColor, marker='3')
   
    x_Axis = list(mix_tot.keys())
    y_Axis_Mix = list(mix_tot.values())
    line_Mix = plt.plot(x_Axis, y_Axis_Mix, label=mixLabel, color=mixColor, marker='x', markersize=7) 
   
    plt.ylim(bottom=0)
    #plt.xlim(left=60)
    plt.legend(loc='lower left')
    plt.savefig("plots/through_lat.png")
    plt.clf()

    print(threshold_tot.keys())
    print(threshold_tot.values())
    print(form_tot.keys())
    print(form_tot.values())
    print(generalized_tot.keys())
    print(generalized_tot.values())
    print(mix_tot.keys())
    print(mix_tot.values())

if __name__ == '__main__':
    with open('res/res_thr_lat.txt') as fp:
        generalized = defaultdict(list)
        threshold = defaultdict(list)
        mix = defaultdict(list)
        form = defaultdict(list)
        for _, line in enumerate(fp):
            ds = line.split('.')
            rate = ds[1]
            c = ds[3]
            sp = line.split()
            thr = float(sp[1])
            lat = float(sp[2])
            if line.startswith("ON"):
                generalized[rate].append((c, thr, lat))
            elif line.startswith("OFF"):
                threshold[rate].append((c, thr, lat))
            elif line.startswith("MIX"):
                mix[rate].append((c, thr, lat))
            elif line.startswith("FORM"):
                form[rate].append((c, thr, lat))
        generalized_tot = dict()
        threshold_tot = dict()
        mix_tot = dict()
        form_tot = dict()
        for key in generalized:
            thr_tot = sum([res_tuple[1] for res_tuple in generalized[key]])
            lat_tot = sum([res_tuple[2] for res_tuple in generalized[key]]) / len(generalized[key])
            generalized_tot[thr_tot] = lat_tot
        for key in threshold:
            thr_tot = sum([res_tuple[1] for res_tuple in threshold[key]])
            lat_tot = sum([res_tuple[2] for res_tuple in threshold[key]]) / len(threshold[key]) 
            threshold_tot[thr_tot] = lat_tot
        for key in mix:
            thr_tot = sum([res_tuple[1] for res_tuple in mix[key]])
            lat_tot = sum([res_tuple[2] for res_tuple in mix[key]]) / len(mix[key]) 
            mix_tot[thr_tot] = lat_tot
        for key in form:
            thr_tot = sum([res_tuple[1] for res_tuple in form[key]])
            lat_tot = sum([res_tuple[2] for res_tuple in form[key]]) / len(form[key]) 
            form_tot[thr_tot] = lat_tot
        # print(generalized)
        # print(generalized_tot)
        # print(threshold)
        # print(threshold_tot)

    # print('through_gen=',[sum(vals) / len(vals) for _ , vals in generalized_throughput.items()])
    # print('through_thresh=',[sum(vals) / len(vals) for _ , vals in threshold_throughput.items()])
    # print('lat_gen=',[sum(vals) / len(vals) for _ , vals in generalized_latency.items()])
    # print('lat_thresh=',[sum(vals) / len(vals) for _ , vals in threshold_latency.items()])
    plot_n()