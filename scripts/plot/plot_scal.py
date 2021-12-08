import sys
import argparse
import numpy as np
from collections import defaultdict
import matplotlib.pyplot as plt

from plot_settings import *

def plot_n():
    plt.grid(True)
    plt.xlabel("Number of replicas")
    plt.ylabel("Throughput (Kops/sec)")
    
    generalized_throughput_avg = defaultdict(list) #avg all runs (:=responses from same client))
    for nodeKey in generalized_throughput:
        for clientKey in generalized_throughput[nodeKey]:
            avgThrOfClient = sum(generalized_throughput[nodeKey][clientKey]) / len(generalized_throughput[nodeKey][clientKey])    
            generalized_throughput_avg[nodeKey].append((clientKey, avgThrOfClient)) #tuple (client id, averaged throughput reported by id)
    
    threshold_throughput_avg = defaultdict(list) #avg all runs (:=responses from same client))
    for nodeKey in threshold_throughput:
        for clientKey in threshold_throughput[nodeKey]:
            avgThrOfClient = sum(threshold_throughput[nodeKey][clientKey]) / len(threshold_throughput[nodeKey][clientKey])    
            threshold_throughput_avg[nodeKey].append((clientKey, avgThrOfClient)) #tuple (client id, averaged throughput reported by id)
    
    mix_throughput_avg = defaultdict(list) #avg all runs (:=responses from same client))
    for nodeKey in mix_throughput:
        for clientKey in mix_throughput[nodeKey]:
            avgThrOfClient = sum(mix_throughput[nodeKey][clientKey]) / len(mix_throughput[nodeKey][clientKey])    
            mix_throughput_avg[nodeKey].append((clientKey, avgThrOfClient)) #tuple (client id, averaged throughput reported by id)
    
    form_throughput_avg = defaultdict(list) #avg all runs (:=responses from same client))
    for nodeKey in form_throughput:
        for clientKey in form_throughput[nodeKey]:
            avgThrOfClient = sum(form_throughput[nodeKey][clientKey]) / len(form_throughput[nodeKey][clientKey])    
            form_throughput_avg[nodeKey].append((clientKey, avgThrOfClient)) #tuple (client id, averaged throughput reported by id)
    # print(generalized_throughput)
    # print(generalized_throughput_avg)
    # print(threshold_throughput)
    # print(threshold_throughput_avg)
    x_Axis = list(map(int, threshold_throughput_avg.keys()))     
    plt.xticks(list(map(int, threshold_throughput_avg.keys())))

    y_Axis_Thresh = [sum([pair[1] for pair in thrs])  for _ , thrs in threshold_throughput_avg.items()]  #sum throughput reported by each client
    y_Axis_Form = [sum([pair[1] for pair in thrs])   for _ , thrs in form_throughput_avg.items()] #thrs is tuple (client id, throughput)
    y_Axis_Gen = [sum([pair[1] for pair in thrs])   for _ , thrs in generalized_throughput_avg.items()] #thrs is tuple (client id, throughput)
    y_Axis_Mix = [sum([pair[1] for pair in thrs])  for _ , thrs in mix_throughput_avg.items()]  #sum throughput reported by each client
   
    line_Thresh = plt.plot(x_Axis, y_Axis_Thresh, label=offLabel, color=offColor, marker='*', markersize=7)
    line_Form = plt.plot(x_Axis, y_Axis_Form, label=formLabel, color=formColor, marker='1')
    line_Gen = plt.plot(x_Axis, y_Axis_Gen, label=onLabel, color=onColor, marker='3')
    line_Mix = plt.plot(x_Axis, y_Axis_Mix, label=mixLabel, color=mixColor, marker='x', markersize=7)
   
    plt.ylim(bottom=0)
    # plt.xlim(left=4)
    # plt.xlim(right=31)
    plt.legend(loc='upper right')
    plt.savefig("plots/throughput.png")
    plt.clf()

    plt.grid(True)
    plt.xlabel("Number of replicas")
    plt.ylabel("Latency (msec)")
    x_Axis = list(map(int, threshold_latency.keys()))     
    plt.xticks(list(map(int, threshold_latency.keys())))
    #print(generalized_latency)
    # print(threshold_latency)
    y_Axis_Thresh = [sum([pair[1] for pair in lats]) / len(lats) for _ , lats in threshold_latency.items()] #avg all clients and all runs (:=responses from same client)
    y_Axis_Form = [sum([pair[1] for pair in lats]) / len(lats) for _ , lats in form_latency.items()]
    y_Axis_Gen = [sum([pair[1] for pair in lats]) / len(lats) for _ , lats in generalized_latency.items()] #lats is tuple (client id, latency)
    y_Axis_Mix= [sum([pair[1] for pair in lats]) / len(lats) for _ , lats in mix_latency.items()]
    
    line_Thresh = plt.plot(x_Axis, y_Axis_Thresh, label=offLabel, color=offColor, marker='*')
    line_Form = plt.plot(x_Axis, y_Axis_Form, label=formLabel, color=formColor, marker='1')
    line_Gen = plt.plot(x_Axis, y_Axis_Gen, label=onLabel, color=onColor, marker='3')
    line_Mix = plt.plot(x_Axis, y_Axis_Mix, label=mixLabel, color=mixColor, marker='x', markersize=8)
    plt.ylim(bottom=0)
    plt.legend(loc='upper left')
    plt.savefig("plots/latency.png")
    
    print([sum([pair[1] for pair in thrs])  for _ , thrs in threshold_throughput_avg.items()] )
    print([sum([pair[1] for pair in thrs])   for _ , thrs in form_throughput_avg.items()] )
    print([sum([pair[1] for pair in thrs])   for _ , thrs in generalized_throughput_avg.items()])
    print([sum([pair[1] for pair in thrs])  for _ , thrs in mix_throughput_avg.items()] )
    print([sum([pair[1] for pair in lats]) / len(lats) for _ , lats in threshold_latency.items()])
    print([sum([pair[1] for pair in lats]) / len(lats) for _ , lats in form_latency.items()])
    print([sum([pair[1] for pair in lats]) / len(lats) for _ , lats in generalized_latency.items()])
    print([sum([pair[1] for pair in lats]) / len(lats) for _ , lats in mix_latency.items()])
    

if __name__ == '__main__':
    with open('res/res_scal.txt') as fp:
        generalized_throughput = defaultdict(list)
        generalized_latency = defaultdict(list)
        threshold_throughput = defaultdict(list)
        threshold_latency = defaultdict(list)
        mix_throughput = defaultdict(list)
        mix_latency = defaultdict(list)
        form_throughput = defaultdict(list)
        form_latency = defaultdict(list)
        for _, line in enumerate(fp):
            if line.startswith('#'):
                continue
            if line.startswith("ON"):
                sp = line.split('.')
                n = sp[1] #num of servers
                c = sp[3] #client id
                val = float(line.split()[1])
                if sp[2] == "thr":
                    if not (n in generalized_throughput.keys()):
                        generalized_throughput[n] = defaultdict(list)
                    generalized_throughput[n][c].append(val)  #Keep a different list of throughputs pro client
                elif sp[2] == "lat":
                    generalized_latency[n].append((c, val)) #tuple (client id, latency  reported by id)
            elif line.startswith("OFF"):
                sp = line.split('.')
                n = sp[1]
                c = sp[3]
                val = float(line.split()[1])
                if sp[2] == "thr":
                    if not (n in threshold_throughput.keys()):
                        threshold_throughput[n] = defaultdict(list)
                    threshold_throughput[n][c].append(val)
                elif sp[2] == "lat":
                    threshold_latency[n].append((c, val))
            elif line.startswith("MIX"):
                sp = line.split('.')
                n = sp[1]
                c = sp[3]
                val = float(line.split()[1])
                if sp[2] == "thr":
                    if not (n in mix_throughput.keys()):
                        mix_throughput[n] = defaultdict(list)
                    mix_throughput[n][c].append(val)
                elif sp[2] == "lat":
                    mix_latency[n].append((c, val))
            elif line.startswith("FORM"):
                sp = line.split('.')
                n = sp[1]
                c = sp[3]
                val = float(line.split()[1])
                if sp[2] == "thr":
                    if not (n in form_throughput.keys()):
                        form_throughput[n] = defaultdict(list)
                    form_throughput[n][c].append(val)
                elif sp[2] == "lat":
                    form_latency[n].append((c, val))
    # print('through_gen=',[sum(vals) / len(vals) for _ , vals in generalized_throughput.items()])
    # print('through_thresh=',[sum(vals) / len(vals) for _ , vals in threshold_throughput.items()])
    # print('lat_gen=',[sum(vals) / len(vals) for _ , vals in generalized_latency.items()])
    # print('lat_thresh=',[sum(vals) / len(vals) for _ , vals in threshold_latency.items()])
    plot_n()