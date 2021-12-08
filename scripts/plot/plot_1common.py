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
    
    mixSimpleQS_throughput_avg = defaultdict(list) #avg all runs (:=responses from same client))
    for nodeKey in mixSimpleQS_throughput:
        for clientKey in mixSimpleQS_throughput[nodeKey]:
            avgThrOfClient = sum(mixSimpleQS_throughput[nodeKey][clientKey]) / len(mixSimpleQS_throughput[nodeKey][clientKey])    
            mixSimpleQS_throughput_avg[nodeKey].append((clientKey, avgThrOfClient)) #tuple (client id, averaged throughput reported by id)        

    form_throughput_avg = defaultdict(list) #avg all runs (:=responses from same client))
    for nodeKey in form_throughput:
        for clientKey in form_throughput[nodeKey]:
            avgThrOfClient = sum(form_throughput[nodeKey][clientKey]) / len(form_throughput[nodeKey][clientKey])    
            form_throughput_avg[nodeKey].append((clientKey, avgThrOfClient)) #tuple (client id, averaged throughput reported by id)
    
    formSimpleQS_throughput_avg = defaultdict(list) #avg all runs (:=responses from same client))
    for nodeKey in formSimpleQS_throughput:
        for clientKey in formSimpleQS_throughput[nodeKey]:
            avgThrOfClient = sum(formSimpleQS_throughput[nodeKey][clientKey]) / len(formSimpleQS_throughput[nodeKey][clientKey])    
            formSimpleQS_throughput_avg[nodeKey].append((clientKey, avgThrOfClient)) #tuple (client id, averaged throughput reported by id)        
    # print(generalized_throughput)
    # print(generalized_throughput_avg)
    # print(threshold_throughput)
    # print(threshold_throughput_avg)
    #y_Axis_Thresh = [sum([pair[1] for pair in thrs])  for _ , thrs in threshold_throughput_avg.items()]  #sum throughput reported by each client  
    #y_Axis_Gen = [sum([pair[1] for pair in thrs])   for _ , thrs in generalized_throughput_avg.items()] #thrs is tuple (client id, throughput)
    #line_Thresh = plt.plot(x_Axis, y_Axis_Thresh, label='Threshold', color='C1', marker='o')
    #line_Gen = plt.plot(x_Axis, y_Axis_Gen, label=onLabel, color='C0', marker='s', lw=1)
    
    x_Axis = list(map(int, form_throughput_avg.keys()))     
    plt.xticks(list(map(int, form_throughput_avg.keys())))
    y_Axis_Form = [sum([pair[1] for pair in thrs])  for _ , thrs in form_throughput_avg.items()] 
    line_Form = plt.plot(x_Axis, y_Axis_Form, label=formLabel+", 2L1C BQS", color=formColor, marker='2')
    
    x_Axis = list(map(int, formSimpleQS_throughput_avg.keys()))     
    plt.xticks(list(map(int, formSimpleQS_throughput_avg.keys())))
    y_Axis_FormSimpleQS = [sum([pair[1] for pair in thrs])  for _ , thrs in formSimpleQS_throughput_avg.items()]  
    line_FormSimpleQS = plt.plot(x_Axis, y_Axis_FormSimpleQS, label=formLabel+", threshold BQS", color=formColor, linestyle='dashed', marker='1')
    
    x_Axis = list(map(int, mix_throughput_avg.keys()))     
    plt.xticks(list(map(int, mix_throughput_avg.keys())))
    y_Axis_Mix = [sum([pair[1] for pair in thrs])  for _ , thrs in mix_throughput_avg.items()]  #sum throughput reported by each client
    line_Mix = plt.plot(x_Axis, y_Axis_Mix, label=mixLabel+", 2L1C BQS", color=mixColor, marker='+')
    
    x_Axis = list(map(int, mixSimpleQS_throughput_avg.keys()))     
    plt.xticks(list(map(int, mixSimpleQS_throughput_avg.keys())))
    y_Axis_MixSimpleQS = [sum([pair[1] for pair in thrs])  for _ , thrs in mixSimpleQS_throughput_avg.items()]  #sum throughput reported by each client
    line_MixSimpleQS = plt.plot(x_Axis, y_Axis_MixSimpleQS, label=mixLabel+", threshold BQS", color=mixColor, linestyle='dashed', marker='x',  markersize=7)

    
    plt.ylim(bottom=0)
    plt.legend(loc='lower right')
    plt.savefig("plots/1common_throughput_msp_repl.png")
    plt.clf()

    plt.grid(True)
    plt.xlabel("Number of replicas")
    plt.ylabel("Latency (msec)")
    # print(generalized_latency)
    # print(threshold_latency)    
    #y_Axis_Thresh = [sum([pair[1] for pair in lats]) / len(lats) for _ , lats in threshold_latency.items()] #avg all clients and all runs (:=responses from same client)
    #y_Axis_Gen = [sum([pair[1] for pair in lats]) / len(lats) for _ , lats in generalized_latency.items()] #lats is tuple (client id, latency)
    #line_Thresh = plt.plot(x_Axis, y_Axis_Thresh, label='Threshold', color='C1', marker='o')
    #ine_Gen = plt.plot(x_Axis, y_Axis_Gen, label=onLabel, color='C0', marker='s', lw=1)

    x_Axis = list(map(int, form_latency.keys()))     
    plt.xticks(list(map(int, form_latency.keys())))
    y_Axis_Form = [sum([pair[1] for pair in lats]) / len(lats) for _ , lats in form_latency.items()]
    line_Form = plt.plot(x_Axis, y_Axis_Form, label=formLabel+", 2L1C BQS", color=formColor, marker='2')
   
    x_Axis = list(map(int, formSimpleQS_latency.keys()))     
    plt.xticks(list(map(int, formSimpleQS_latency.keys())))
    y_Axis_FormSimpleQS= [sum([pair[1] for pair in lats]) / len(lats) for _ , lats in formSimpleQS_latency.items()]
    line_FormSimpleQS = plt.plot(x_Axis, y_Axis_FormSimpleQS, label=formLabel+", threshold BQS", linestyle='dashed', color=formColor, marker='1') 
    
    x_Axis = list(map(int, mix_latency.keys()))     
    plt.xticks(list(map(int, mix_latency.keys())))
    y_Axis_Mix= [sum([pair[1] for pair in lats]) / len(lats) for _ , lats in mix_latency.items()] #avg all clients and all runs (:=responses from same client)
    line_Mix = plt.plot(x_Axis, y_Axis_Mix, label=mixLabel+", 2L1C BQS", color=mixColor, marker='+')  #lats is tuple (client id, latency)
    
    x_Axis = list(map(int, mixSimpleQS_latency.keys()))     
    plt.xticks(list(map(int, mixSimpleQS_latency.keys())))
    y_Axis_MixSimpleQS= [sum([pair[1] for pair in lats]) / len(lats) for _ , lats in mixSimpleQS_latency.items()]
    line_MixSimpleQS = plt.plot(x_Axis, y_Axis_MixSimpleQS, label=mixLabel+", threshold BQS", linestyle='dashed', color=mixColor, marker='x',  markersize=7)
    
    plt.ylim(bottom=0)
    plt.legend(loc='lower left')
    plt.savefig("plots/1common_latency_msp_repl.png")


if __name__ == '__main__':
    with open('res/res_1common.txt') as fp:
        generalized_throughput = defaultdict(list)
        generalized_latency = defaultdict(list)
        threshold_throughput = defaultdict(list)
        threshold_latency = defaultdict(list)
        mix_throughput = defaultdict(list)
        mix_latency = defaultdict(list)
        mixSimpleQS_throughput = defaultdict(list)
        mixSimpleQS_latency = defaultdict(list)
        form_throughput = defaultdict(list)
        form_latency = defaultdict(list)
        formSimpleQS_throughput = defaultdict(list)
        formSimpleQS_latency = defaultdict(list)
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
            elif line.startswith("TMIX"):
                sp = line.split('.')
                n = sp[1]
                c = sp[3]
                val = float(line.split()[1])
                if sp[2] == "thr":
                    if not (n in mixSimpleQS_throughput.keys()):
                        mixSimpleQS_throughput[n] = defaultdict(list)
                    mixSimpleQS_throughput[n][c].append(val)
                elif sp[2] == "lat":
                    mixSimpleQS_latency[n].append((c, val))
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
            elif line.startswith("TFORM"):
                sp = line.split('.')
                n = sp[1]
                c = sp[3]
                val = float(line.split()[1])
                if sp[2] == "thr":
                    if not (n in formSimpleQS_throughput.keys()):
                        formSimpleQS_throughput[n] = defaultdict(list)
                    formSimpleQS_throughput[n][c].append(val)
                elif sp[2] == "lat":
                    formSimpleQS_latency[n].append((c, val))
    # print('through_gen=',[sum(vals) / len(vals) for _ , vals in generalized_throughput.items()])
    # print('through_thresh=',[sum(vals) / len(vals) for _ , vals in threshold_throughput.items()])
    # print('lat_gen=',[sum(vals) / len(vals) for _ , vals in generalized_latency.items()])
    # print('lat_thresh=',[sum(vals) / len(vals) for _ , vals in threshold_latency.items()])
    plot_n()