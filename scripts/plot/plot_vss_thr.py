import sys
import argparse
import numpy as np
from collections import defaultdict
import matplotlib.pyplot as plt

from plot_settings import *

width = 1

dataShare = defaultdict(list) # dataShare["THR"] = [], dataShare["GEN"] = []
dataVerify = defaultdict(list)
dataRecons = defaultdict(list)
dataGenerate = defaultdict(list)
dataKeygen = defaultdict(list)
dataCombine = defaultdict(list)
parties = []

def plot(data, filename, withLegend=True):
    plt.bar(parties, data["THR"], width=width, )
    plt.bar([n + width for n in parties], data["GEN"], width=width)
    plt.bar([n + 2*width for n in parties], data["THRCOMPL"], width=width)
    plt.bar([n + 3*width for n in parties], data["GENCOMPL"], width=width)
    plt.xlabel("number of parties")
    plt.ylabel("time (ms)")
    if withLegend:
        leg = ['Pedersen n/2', 'Generalized n/2', 'Pedersen 2n/3', 'Generalized 2n/3']
        plt.legend(leg, loc='upper left')
    plt.grid(True)
    plt.show()
    plt.savefig(filename)
    plt.clf()

if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise ValueError('vss/coin?')

    infile = ''    
    if sys.argv[1] == "vss":
        infile = 'res/microbench_vss_thr.txt'
    if sys.argv[1] == "coin":
        infile = 'res/microbench_coin_thr.txt'
    
    with open(infile) as fp:
        for _, line in enumerate(fp):
            if line.startswith('#'):
                continue
            value = float(line.split()[1])
            trustType = line.split('.')[0]
            n = line.split('.')[1]
            algo = line.split('.')[2]
            if algo == "SHARE": #vss
                dataShare[trustType].append(value)
            elif algo == "VERIF": #vss and coin
                dataVerify[trustType].append(value)
            elif algo == "RECON": #vss
                dataRecons[trustType].append(value)
            elif algo == "KEYGEN": #coin
                dataKeygen[trustType].append(value)
            elif algo == "GENER": #coin
                dataGenerate[trustType].append(value)
            elif algo == "COMB": #coin
                dataCombine[trustType].append(value)
            if algo == "VERIF" and trustType == "THR":
                parties.append(int(n)) # n expected to be the same for all comb. of trustType and algo, and in ascending order
    
    if sys.argv[1] == "vss":
        plot(dataShare, "plots/vss_threshold_assumption_SHARE.png", True)
        plot(dataVerify, "plots/vss_threshold_assumption_VERIFY.png", True)
        plot(dataRecons, "plots/vss_threshold_assumption_RECONSTRUCT.png", True)
    elif sys.argv[1] == "coin":
        plot(dataKeygen, "plots/coin_threshold_assumption_KEYGEN.png", True)
        plot(dataGenerate, "plots/coin_threshold_assumption_GENERATE.png", True)
        plot(dataVerify, "plots/coin_threshold_assumption_VERIFY.png", True)
        plot(dataCombine, "plots/coin_threshold_assumption_COMBINE.png", True)
    
    
    