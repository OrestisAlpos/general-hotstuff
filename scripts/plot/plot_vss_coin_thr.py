import sys
import argparse
import numpy as np
from collections import defaultdict
import matplotlib.pyplot as plt

from plot_settings import *

configurations = ["THR", "THRCOMPL", "GEN", "GENCOMPL", "UNBAL", "GEN_MAJ", "GEN_UNBAL", "GEN_KGRID"]

dataSign = defaultdict(dict)
dataShare = defaultdict(dict) # dataShare["THR"] = [], dataShare["GEN"] = []
dataShareComputeShares = defaultdict(dict)
dataShareComputeCommitments = defaultdict(dict)
dataVerify = defaultdict(dict)
dataRecons = defaultdict(dict)
dataGenerate = defaultdict(dict)
dataKeygen = defaultdict(dict)
dataCombine = defaultdict(dict)
dataAuthGroupSize = defaultdict(dict)
dataActualMspRows = defaultdict(dict)
dataBitsInRecomb = defaultdict(dict)
dataMspSize = defaultdict(dict)

for configuration in configurations:
    dataSign[configuration] = defaultdict(list)
    dataShare[configuration] = defaultdict(list)
    dataShareComputeShares[configuration] = defaultdict(list)
    dataShareComputeCommitments[configuration] = defaultdict(list)
    dataVerify[configuration] = defaultdict(list)
    dataRecons[configuration] = defaultdict(list)
    dataGenerate[configuration] = defaultdict(list)
    dataKeygen[configuration] = defaultdict(list)
    dataCombine[configuration] = defaultdict(list)
    dataAuthGroupSize[configuration] = defaultdict(list)    
    dataActualMspRows[configuration] = defaultdict(list)    
    dataBitsInRecomb[configuration] = defaultdict(list)    
    dataMspSize[configuration] = defaultdict(list)    

pattern=dict()
pattern["THR"] = "."
# pattern["THRCOMPL"] = "o"
pattern["GEN"]="-"
# pattern["GENCOMPL"]="+"
color=dict()
color["THR"] = "lightgray"
# color["THRCOMPL"] = "darkgrey"
color["GEN"]="dodgerblue"
# color["GENCOMPL"]="royalblue"
color["UNBAL"]="royalblue"

color["GEN_MAJ"]=color["GEN"]="dodgerblue"
color["GEN_UNBAL"]=color["UNBAL"]
color["GEN_KGRID"]="darkblue"

marker=dict()
marker["THR"] = "x"
# marker["THRCOMPL"] = "*"
marker["GEN"]="."
# marker["GENCOMPL"]="p"
marker["UNBAL"]="p"

marker["GEN_MAJ"]=marker["GEN"]="."
marker["GEN_UNBAL"]=marker["UNBAL"]="p"
marker["GEN_KGRID"]="x"

legend=dict()
legend["THR"] = "Threshold n/2"
# legend["THRCOMPL"] = "Threshold  2n/3"
legend["GEN"]="General n/2"
# legend["GENCOMPL"]="General 2n/3"
legend["UNBAL"]="General Unbalanced"

legend["GEN_MAJ"]=legend["GEN"]
legend["GEN_UNBAL"]=legend["UNBAL"]
legend["GEN_KGRID"]="General Grid"

legend

def plot(data, configurationsToPlot, filename, withLegend=True, ylabel="time (ms)", scaleChange = 1):
    for configuration in configurationsToPlot:
        plt.errorbar(
            x=[k for k in data[configuration].keys()],
            y=[np.mean(d) / scaleChange for d in data[configuration].values()], #each d is a list of time durations
            yerr=[np.std(d) / scaleChange for d in data[configuration].values()], 
            color=color[configuration], 
            marker=marker[configuration],
            capsize=3)
    plt.xticks([k for k in data[configuration].keys()]) # show only values n for which we have a point
    plt.xlabel("number of parties")
    plt.ylabel(ylabel)
    leg=[]
    if withLegend:
        # if sys.argv[1] == "vss":
        #     leg = ['Threshold VSS n/2', 'Generalized VSS n/2', 'Threshold VSS 2n/3', 'Generalized VSS 2n/3']
        # elif sys.argv[1] == "coin":
        #     leg = ['Threshold coin n/2', 'Generalized coin n/2', 'Threshold coin 2n/3', 'Generalized coin 2n/3']
        # elif sys.argv[1] == "bls":
        #     leg = ['Threshold BLS n/2', 'Generalized BLS n/2', 'Threshold BLS 2n/3', 'Generalized BLS 2n/3']
        for configuration in configurationsToPlot:
            leg.append(legend[configuration])
        plt.legend(leg, loc='upper left')
    plt.grid(True)
    plt.show()
    plt.savefig(filename)
    plt.clf()

def plotComparativeBar(data1, data2, label1, label2, configurationsToPlot, filename, withLegend=True):
    # leg=[]
    width=4
    i=0
    fig, ax = plt.subplots()
    for configuration in configurationsToPlot:
        height1 = [np.mean(d) for d in data1[configuration].values()]
        ax.bar(
            x=[k - width/2 + i*width  for k in data1[configuration].keys()], 
            height=height1, 
            width=width,
            yerr=[np.std(d) for d in data1[configuration].values()], 
            color=color[configuration],
            edgecolor="black",
            hatch='x',
            label=legend[configuration] + " " + label1 
            # marker=marker[configuration],
            # capsize=3
            )
        # leg.append(legend[configuration] + " Shares")
        ax.bar(
            x=[k - width/2 + i*width  for k in data2[configuration].keys()], 
            height=[np.mean(d) for d in data2[configuration].values()], 
            width=width,
            yerr=[np.std(d) for d in data2[configuration].values()],
            bottom=height1,
            color=color[configuration],
            label=legend[configuration] + " " + label2
            # marker=marker[configuration],
            # capsize=3
            )
        i += 1
        # leg.append(legend[configuration] + " Commitments")
    plt.xticks([k for k in data1[configuration].keys()])
    ax.set_xlabel("number of parties")
    ax.set_ylabel("time (ms)")
    ax.set_yscale("log")
    ax.set_ylim(ax.get_ylim()[0], ax.get_ylim()[1] * 10)
    if withLegend:
        ax.legend(loc='upper left')
    plt.grid(True)
    plt.show()
    plt.savefig(filename)
    plt.clf()

def plotComparativeLine(data1, data2, configurationsToPlot, filename, withLegend=True):
    # leg=[]
    fig, ax = plt.subplots()
    for configuration in configurationsToPlot:
        ax.errorbar(
            x=[k for k in data1[configuration].keys()], 
            y=[np.mean(d) for d in data1[configuration].values()],
            yerr=[np.std(d) for d in data1[configuration].values()],
            color=color[configuration],
            marker=marker[configuration],
            capsize=3,
            label=legend[configuration] + " Data1")
        ax.set_xlabel("number of parties")
        ax.set_ylabel("time (ms)")
        
        ax2 = ax.twinx()
        ax2.errorbar(
            x=[k for k in data2[configuration].keys()], 
            y=[np.mean(d) for d in data2[configuration].values()], 
            yerr=[np.std(d) for d in data2[configuration].values()],
            color=color[configuration],
            linestyle="dashed",
            marker=marker[configuration],
            capsize=3,
            label=legend[configuration] + " Data2")
        # leg.append(legend[configuration] + " Commitments")    
    # ax.set_yscale("log")
    if withLegend:
        ax.legend(loc='upper left')
    plt.grid(True)
    plt.show()
    plt.savefig(filename)
    plt.clf()

# blue, orange, green, red
def plotHist(dataList, filename, withLegend=True):
    # leg=[]
    fig, ax = plt.subplots()
    ax.hist(
        x=dataList,
        bins=50,
        histtype="bar",
        # color=color[configuration],
        # marker=marker[configuration],
        # capsize=3,
        # label=legend[configuration] + " Data1"
    )
    ax.set_xlabel("number of parties")
    # if withLegend:
    #     ax.legend(loc='upper left')
    plt.grid(True)
    plt.show()
    plt.savefig(filename)
    plt.clf()



if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise ValueError('vss/coin/bls?')

    infile = ''    
    if sys.argv[1] == "vss":
        infile = 'res/microbench_vss.txt'
    elif sys.argv[1] == "coin":
        infile = 'res/microbench_coin.txt'
    elif sys.argv[1] == "bls":
        infile = 'res/microbench_bls.txt'
    elif sys.argv[1] == "auth":
        infile = 'res/microbench_auth.txt'
    else:
        raise ValueError('vss/coin/bls/auth?')
    
    with open(infile) as fp:
        for _, line in enumerate(fp):
            if line.startswith('#'):
                continue
            value = float(line.split()[1])
            configuration = line.split('.')[0]
            n = line.split('.')[1]
            algo = line.split('.')[2]
            if algo == "SHARE": #vss
                dataShare[configuration][int(n)].append(value)
            if algo == "SHARE_SHARES": #vss
                dataShareComputeShares[configuration][int(n)].append(value)
            if algo == "SHARE_COMMS": #vss
                dataShareComputeCommitments[configuration][int(n)].append(value)
            elif algo == "VERIF": #vss and coin and bls
                dataVerify[configuration][int(n)].append(value)
            elif algo == "RECON": #vss
                dataRecons[configuration][int(n)].append(value)
            elif algo == "KEYGEN": #coin
                dataKeygen[configuration][int(n)].append(value)
            elif algo == "GENER": #coin
                dataGenerate[configuration][int(n)].append(value)
            elif algo == "COMB": #coin and bls
                dataCombine[configuration][int(n)].append(value)
            elif algo == "SIGN": #bls
                dataSign[configuration][int(n)].append(value)
            elif algo == "AUTH_SIZE": #auth
                dataAuthGroupSize[configuration][int(n)].append(value)
            elif algo == "ACTUAL_MSP_ROWS": #auth
                dataActualMspRows[configuration][int(n)].append(value)
            elif algo == "TOTAL_BITS": #auth
                dataBitsInRecomb[configuration][int(n)].append(value)
            elif algo == "MSP_SIZE": #auth
                dataMspSize[configuration][int(n)].append(value)
            # if algo == "VERIF" and configuration == "THR":
            #     parties.append(int(n)) # n expected to be the same for all comb. of configuration and algo, and in ascending order
    
    if sys.argv[1] == "vss":
        plot(dataShare, ["THR", "GEN", "UNBAL"], "plots/vss_SHARE.png",  True)
        plot(dataVerify, ["THR", "GEN", "UNBAL"], "plots/vss_VERIFY.png", True)
        plot(dataRecons, ["THR", "GEN", "UNBAL"], "plots/vss_RECONSTRUCT.png", True)

        plot(dataShare, ["THR", "GEN_MAJ", "GEN_UNBAL", "GEN_KGRID"], "plots/vss_SHARE_squares.png",  True)
        plot(dataVerify, ["THR", "GEN_MAJ", "GEN_UNBAL", "GEN_KGRID"], "plots/vss_VERIFY_squares.png",  True)
        plot(dataRecons, ["THR", "GEN_MAJ", "GEN_UNBAL", "GEN_KGRID"], "plots/vss_RECONSTRUCT_squares.png",  True)

        plotComparativeBar(dataShareComputeShares, dataShareComputeCommitments, "Compute shares", "Compute commitments", ["GEN_UNBAL", "GEN_KGRID"], "plots/vss_SHARE_shares_vs_commitments.png", True)
        plot(dataShareComputeShares, ["THR", "GEN_MAJ", "GEN_UNBAL", "GEN_KGRID"], "plots/vss_SHARE_shares.png", True)
        plotHist([ dataRecons["GEN_MAJ"][100], dataRecons["GEN_KGRID"][100] ], "plots/vss_RECONSTRUCT_timeHist.png", True)

    elif sys.argv[1] == "coin":
        plot(dataKeygen, ["THR", "GEN", "UNBAL"], "plots/coin_KEYGEN.png", True)
        plot(dataGenerate, ["THR", "GEN", "UNBAL"], "plots/coin_GENERATE.png", True)
        plot(dataVerify, ["THR", "GEN", "UNBAL"], "plots/coin_VERIFY.png", True)
        plot(dataCombine, ["THR", "GEN", "UNBAL"], "plots/coin_COMBINE.png", True)
        
        plot(dataKeygen, ["THR", "GEN_MAJ", "GEN_UNBAL", "GEN_KGRID"], "plots/coin_KEYGEN_squares.png", True)
        plot(dataGenerate, ["THR", "GEN_MAJ", "GEN_UNBAL", "GEN_KGRID"], "plots/coin_GENERATE_squares.png", True)
        plot(dataVerify, ["THR", "GEN_MAJ", "GEN_UNBAL", "GEN_KGRID"], "plots/coin_VERIFY_squares.png", True)
        plot(dataCombine, ["THR", "GEN_MAJ", "GEN_UNBAL", "GEN_KGRID"], "plots/coin_COMBINE_squares.png", True)

        plotComparativeLine(dataCombine, dataAuthGroupSize, ["GEN_UNBAL", "GEN_KGRID"], "plots/coin_COMBINE_time_vs_authGroupSize.png", True)
        
        plotHist([ dataAuthGroupSize["GEN_MAJ"][100], dataAuthGroupSize["GEN_KGRID"][100] ], "plots/coin_COMBINE_authSizeHist.png", True)
        plotHist([ dataCombine["GEN_MAJ"][100], dataCombine["GEN_KGRID"][100] ], "plots/coin_COMBINE_timeHist.png", True)
    elif sys.argv[1] == "bls":
        plot(dataSign, ["THR", "GEN", "UNBAL"], "plots/bls_SIGN.png", True)
        plot(dataVerify, ["THR", "GEN", "UNBAL"], "plots/bls_VERIFY.png", True)
        plot(dataCombine, ["THR", "GEN", "UNBAL"], "plots/bls_COMBINE.png", True)
        
        plot(dataSign, ["THR", "GEN_MAJ", "GEN_UNBAL", "GEN_KGRID"], "plots/bls_SIGN_squares.png", True)
        plot(dataVerify, ["THR", "GEN_MAJ", "GEN_UNBAL", "GEN_KGRID"], "plots/bls_VERIFY_squares.png", True)
        plot(dataCombine, ["THR", "GEN_MAJ", "GEN_UNBAL", "GEN_KGRID"], "plots/bls_COMBINE_squares.png", True)
   
    elif sys.argv[1] == "auth":
        plot(dataAuthGroupSize, ["GEN_MAJ", "GEN_UNBAL", "GEN_KGRID"], "plots/auth_SIZE.png", True, ylabel="number of parties in authorized set")
        plot(dataActualMspRows, ["GEN_MAJ", "GEN_UNBAL", "GEN_KGRID"], "plots/auth_ACTUAL_SIZE.png", True, ylabel="number of shares")    
        plot(dataBitsInRecomb, ["GEN_MAJ", "GEN_UNBAL", "GEN_KGRID"], "plots/auth_TOTAL_BITS.png", True, ylabel="Kbits", scaleChange=1000)    
        plot(dataMspSize, ["GEN_MAJ", "GEN_UNBAL", "GEN_KGRID"], "plots/auth_MSP_SIZE.png", True, ylabel="KB", scaleChange=1000)    
    
     