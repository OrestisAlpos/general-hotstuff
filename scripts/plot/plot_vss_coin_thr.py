import sys
import argparse
import numpy as np
from collections import defaultdict
import matplotlib.pyplot as plt

from plot_settings import *

trustTypes = ["THR", "THRCOMPL", "GEN", "GENCOMPL", "UNBAL"]
trustTypesToPlot = ["THR", "GEN", "UNBAL"]

dataSign = defaultdict(dict)
dataShare = defaultdict(dict) # dataShare["THR"] = [], dataShare["GEN"] = []
dataVerify = defaultdict(dict)
dataRecons = defaultdict(dict)
dataGenerate = defaultdict(dict)
dataKeygen = defaultdict(dict)
dataCombine = defaultdict(dict)

for trustType in trustTypes:
    dataSign[trustType] = defaultdict(list)
    dataShare[trustType] = defaultdict(list)
    dataVerify[trustType] = defaultdict(list)
    dataRecons[trustType] = defaultdict(list)
    dataGenerate[trustType] = defaultdict(list)
    dataKeygen[trustType] = defaultdict(list)
    dataCombine[trustType] = defaultdict(list)    

pattern=dict()
pattern["THR"] = "."
pattern["THRCOMPL"] = "o"
pattern["GEN"]="-"
pattern["GENCOMPL"]="+"
color=dict()
color["THR"] = "lightgray"
color["THRCOMPL"] = "darkgrey"
color["GEN"]="dodgerblue"
color["GENCOMPL"]="royalblue"
color["UNBAL"]="royalblue"
marker=dict()
marker["THR"] = "x"
marker["THRCOMPL"] = "*"
marker["GEN"]="."
marker["GENCOMPL"]="p"
marker["UNBAL"]="p"
legend=dict()
legend["THR"] = "Threshold n/2"
legend["THRCOMPL"] = "Threshold  2n/3"
legend["GEN"]="Generalized n/2"
legend["GENCOMPL"]="Generalized 2n/3"
legend["UNBAL"]="Generalized Unbalanced"
legend

def plot(data, filename, withLegend=True):
    for trustType in trustTypesToPlot:
        plt.errorbar(
            x=[k for k in data[trustType].keys()], 
            y=[np.mean(d) for d in data[trustType].values()], 
            yerr=[np.std(d) for d in data[trustType].values()], 
            color=color[trustType], 
            marker=marker[trustType],
            capsize=3)
    plt.xlabel("number of parties")
    plt.ylabel("time (ms)")
    leg=[]
    if withLegend:
        # if sys.argv[1] == "vss":
        #     leg = ['Threshold VSS n/2', 'Generalized VSS n/2', 'Threshold VSS 2n/3', 'Generalized VSS 2n/3']
        # elif sys.argv[1] == "coin":
        #     leg = ['Threshold coin n/2', 'Generalized coin n/2', 'Threshold coin 2n/3', 'Generalized coin 2n/3']
        # elif sys.argv[1] == "bls":
        #     leg = ['Threshold BLS n/2', 'Generalized BLS n/2', 'Threshold BLS 2n/3', 'Generalized BLS 2n/3']
        for trustType in trustTypesToPlot:
            leg.append(legend[trustType])
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
        # infile = 'res/microbench_vss_thr.txt'
        infile = 'res/microbench_vss.txt'
    elif sys.argv[1] == "coin":
        # infile = 'res/microbench_coin_thr.txt'
        infile = 'res/microbench_coin.txt'
    elif sys.argv[1] == "bls":
        # infile = 'res/microbench_bls_thr.txt'
        infile = 'res/microbench_bls.txt'
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
                dataShare[trustType][int(n)].append(value)
            elif algo == "VERIF": #vss and coin and bls
                dataVerify[trustType][int(n)].append(value)
            elif algo == "RECON": #vss
                dataRecons[trustType][int(n)].append(value)
            elif algo == "KEYGEN": #coin
                dataKeygen[trustType][int(n)].append(value)
            elif algo == "GENER": #coin
                dataGenerate[trustType][int(n)].append(value)
            elif algo == "COMB": #coin and bls
                dataCombine[trustType][int(n)].append(value)
            elif algo == "SIGN": #bls
                dataSign[trustType][int(n)].append(value)
            # if algo == "VERIF" and trustType == "THR":
            #     parties.append(int(n)) # n expected to be the same for all comb. of trustType and algo, and in ascending order
    
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
    
    
    