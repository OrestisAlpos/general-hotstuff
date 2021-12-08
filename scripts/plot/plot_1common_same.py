import sys
import argparse
import numpy as np
from collections import defaultdict
import matplotlib.pyplot as plt
import matplotlib.lines as mlines

offLabel = "Counting-HotStuff"
formLabel = "MBF-HotStuff"
onLabel = "MSP-HotStuff"
mixLabel = "MSP-Replicas"
plt.rcParams['lines.markersize'] = 10
plt.rcParams['lines.linewidth'] = 1.25

#https://stackoverflow.com/questions/8409095/matplotlib-set-markers-for-individual-points-on-a-line
def plot_n():
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
        
    fig, ax1 = plt.subplots()
    ax2 = ax1.twinx()

    ax1.grid(False)
    ax1.set_xlabel("Number of replicas")
    x_Axis = list(map(int, form_throughput_avg.keys()))     
    ax1.set_xticks(list(map(int, form_throughput_avg.keys())))

    #THROUGHPOUT
    y_Axis_Form = [sum([pair[1] for pair in thrs])  for _ , thrs in form_throughput_avg.items()] 
    line_Form, = ax1.plot(x_Axis, y_Axis_Form, label=formLabel+", 2L1C BQS", color='black', marker='2')
    
    y_Axis_FormSimpleQS = [sum([pair[1] for pair in thrs])  for _ , thrs in formSimpleQS_throughput_avg.items()]  
    line_FormSimpleQS, = ax1.plot(x_Axis, y_Axis_FormSimpleQS, label=formLabel+", threshold BQS", color='C3', marker='1')
    
    y_Axis_Mix = [sum([pair[1] for pair in thrs])  for _ , thrs in mix_throughput_avg.items()]  #sum throughput reported by each client
    line_Mix, = ax1.plot(x_Axis, y_Axis_Mix, label=mixLabel+", 2L1C BQS", color='gray', marker='+')
    
    y_Axis_MixSimpleQS = [sum([pair[1] for pair in thrs])  for _ , thrs in mixSimpleQS_throughput_avg.items()]  #sum throughput reported by each client
    line_MixSimpleQS, = ax1.plot(x_Axis, y_Axis_MixSimpleQS, label=mixLabel+", threshold BQS", color='C2', marker='x', markersize=7)

    ax1.set_ylabel("Throughput (Kops/sec)")
    ax1.set_ylim(bottom=0)
    #ax1.legend(loc='lower left')
    
    #LATENCY
    y_Axis_Form = [sum([pair[1] for pair in lats]) / len(lats) for _ , lats in form_latency.items()]
    line_Form_lat, = ax2.plot(x_Axis, y_Axis_Form, linestyle='dashed', label=formLabel+", 2L1C BQS", color='black', marker='2')
   
    y_Axis_FormSimpleQS= [sum([pair[1] for pair in lats]) / len(lats) for _ , lats in formSimpleQS_latency.items()]
    line_FormSimpleQS_lat, = ax2.plot(x_Axis, y_Axis_FormSimpleQS, linestyle='dashed', label=formLabel+", threshold BQS", color='C3', marker='1') 
    
    y_Axis_Mix= [sum([pair[1] for pair in lats]) / len(lats) for _ , lats in mix_latency.items()] #avg all clients and all runs (:=responses from same client)
    line_Mix_lat, = ax2.plot(x_Axis, y_Axis_Mix, linestyle='dashed', label=mixLabel+", 2L1C BQS", color='gray', marker='+')  #lats is tuple (client id, latency)
    
    y_Axis_MixSimpleQS= [sum([pair[1] for pair in lats]) / len(lats) for _ , lats in mixSimpleQS_latency.items()]
    line_MixSimpleQS_lat, = ax2.plot(x_Axis, y_Axis_MixSimpleQS, linestyle='dashed', label=mixLabel+", threshold BQS", color='C2', marker='x', markersize=7)
    
    ax2.set_ylabel("Latency (msec)")
    ax2.set_ylim(bottom=0)
    #ax2.legend(loc='lower right')
    
    solid_patch = mlines.Line2D([], [], color='black', marker=None, label='Throughput')
    dashed_patch = mlines.Line2D([], [], color='black', marker=None, linestyle='dashed', label='Latency')
    first_legend = plt.legend(handles=[line_Form, line_FormSimpleQS, line_Mix, line_MixSimpleQS], 
                              labels=[formLabel+", 2L1C BQS", formLabel+", threshold BQS", mixLabel+", 2L1C BQS", mixLabel+", threshold BQS"], 
                              loc='lower left')
    plt.gca().add_artist(first_legend)
    plt.legend(handles=[solid_patch, dashed_patch], labels=["Throughput", "Latency"], loc='lower right')

    plt.savefig("plots/1common_same.png")


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