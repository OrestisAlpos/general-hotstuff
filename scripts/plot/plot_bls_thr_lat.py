import sys
import argparse
import numpy as np
from collections import defaultdict
import matplotlib.pyplot as plt

from plot_settings import *

if __name__ == '__main__':
    plt.grid(True)
    plt.xlabel("Throughput (Kops/sec)")
    plt.ylabel("Latency (msec)")

    parser = argparse.ArgumentParser()
    parser.add_argument('confs', nargs='+')
    args = parser.parse_args()
    for target_conf in args.confs:
        print('Plotting throughput-latency for configuration:', target_conf)
        
        #Read results for this configuration
        with open('res/res_bls_thr_lat.txt') as fp:
            results = defaultdict(list)
            for _, line in enumerate(fp):
                ds = line.split('.')
                conf = ds[0]
                rate = ds[1]
                c = ds[3]
                sp = line.split()
                thr = float(sp[1])
                lat = float(sp[2])
                if conf == target_conf:
                    results[int(rate)].append((c, thr, lat))
        
        #Calculate aggregate results
        results_tot = dict()
        for key in sorted(results):
            thr_tot = sum([res_tuple[1] for res_tuple in results[key]])
            lat_tot = sum([res_tuple[2] for res_tuple in results[key]]) / len(results[key])
            results_tot[thr_tot] = lat_tot
        
        #Plot line for this configuratrion
        x_Axis = list(results_tot.keys())
        y_Axis = list(results_tot.values())
        line = plt.plot(x_Axis, y_Axis, label=target_conf) 
        print(results_tot.keys())
        print(results_tot.values())

    plt.ylim(bottom=0)
    #plt.xlim(left=60)
    plt.legend(loc='lower left')
    plt.savefig("plots/through_lat.png")
    plt.clf()