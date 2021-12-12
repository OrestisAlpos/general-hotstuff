import sys
import argparse
import numpy as np
from collections import defaultdict
import matplotlib.pyplot as plt

from plot_settings import *

dataShare = defaultdict(list) # dataShare["THR"] = [], dataShare["GEN"] = []
dataVerify = defaultdict(list)
dataRecons = defaultdict(list)
dataGenerate = defaultdict(list)
dataKeygen = defaultdict(list)
dataCombine = defaultdict(list)
dataSign = defaultdict(list)
parties = []

patternThrSimple = "."
patternThrCompl = "o"
patternGenSimple="-"
patternGenCompl="+"
colorThrSimple = "lightgray"
colorThrCompl = "darkgrey"
colorGenSimple="dodgerblue"
colorGenCompl="royalblue"
markerThrSimple = "x"
markerThrCompl = "*"
markerGenSimple="."
markerGenCompl="p"
width = 2


def plot(data, filename, withLegend=True):
    # plt.bar(parties, data["THR"], width=width, color=colorThrSimple, hatch=patternThrSimple)
    # plt.bar([n + width for n in parties], data["GEN"], width=width, color=colorGenSimple, hatch=patternGenSimple)
    # plt.bar([n + 2*width for n in parties], data["THRCOMPL"], width=width, color=colorThrCompl, hatch=patternThrCompl)
    # plt.bar([n + 3*width for n in parties], data["GENCOMPL"], width=width, color=colorGenCompl, hatch=patternGenCompl)
    plt.plot(parties, data["THR"], color=colorThrSimple, marker=markerThrSimple)
    plt.plot(parties, data["GEN"], color=colorGenSimple, marker=markerGenSimple)
    # plt.plot(parties, data["THRCOMPL"], color=colorThrCompl, marker=markerThrCompl)
    # plt.plot(parties, data["GENCOMPL"], color=colorGenCompl, marker=markerGenCompl)
    plt.xticks(parties)
    plt.xlabel("number of parties")
    plt.ylabel("time (ms)")
    if withLegend:
        if sys.argv[1] == "vss":
            leg = ['Threshold VSS n/2', 'Generalized VSS n/2', 'Threshold VSS 2n/3', 'Generalized VSS 2n/3']
        elif sys.argv[1] == "coin":
            leg = ['Threshold coin n/2', 'Generalized coin n/2', 'Threshold coin 2n/3', 'Generalized coin 2n/3']
        elif sys.argv[1] == "bls":
            leg = ['Threshold BLS n/2', 'Generalized BLS n/2', 'Threshold BLS 2n/3', 'Generalized BLS 2n/3']
        plt.legend(leg, loc='upper left')
    plt.grid(True)
    plt.show()
    plt.savefig(filename)
    plt.clf()

if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise ValueError('vss/coin/bls?')

    infile = ''    
    if sys.argv[1] == "vss":
        infile = 'res/microbench_vss_thr.txt'
    elif sys.argv[1] == "coin":
        infile = 'res/microbench_coin_thr.txt'
    elif sys.argv[1] == "bls":
        infile = 'res/microbench_bls_thr.txt'
    else:
        raise ValueError('vss/coin/bls?')
    
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
            elif algo == "VERIF": #vss and coin and bls
                dataVerify[trustType].append(value)
            elif algo == "RECON": #vss
                dataRecons[trustType].append(value)
            elif algo == "KEYGEN": #coin
                dataKeygen[trustType].append(value)
            elif algo == "GENER": #coin
                dataGenerate[trustType].append(value)
            elif algo == "COMB": #coin and bls
                dataCombine[trustType].append(value)
            elif algo == "SIGN": #bls
                dataSign[trustType].append(value)
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
    elif sys.argv[1] == "bls":
        plot(dataSign, "plots/bls_threshold_assumption_SIGN.png", True)
        plot(dataVerify, "plots/bls_threshold_assumption_VERIFY.png", True)
        plot(dataCombine, "plots/bls_threshold_assumption_COMBINE.png", True)
    
    
    