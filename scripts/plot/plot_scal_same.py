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
    
    form_throughput_avg = defaultdict(list) #avg all runs (:=responses from same client))
    for nodeKey in form_throughput:
        for clientKey in form_throughput[nodeKey]:
            avgThrOfClient = sum(form_throughput[nodeKey][clientKey]) / len(form_throughput[nodeKey][clientKey])    
            form_throughput_avg[nodeKey].append((clientKey, avgThrOfClient)) #tuple (client id, averaged throughput reported by id)
    # print(generalized_throughput)
    # print(generalized_throughput_avg)
    # print(threshold_throughput)
    # print(threshold_throughput_avg)
    
    fig, ax1 = plt.subplots()
    ax2 = ax1.twinx()

    ax1.grid(False)
    ax1.set_xlabel("Number of replicas")
    x_Axis = list(map(int, threshold_throughput_avg.keys()))     
    ax1.set_xticks(list(map(int, threshold_throughput_avg.keys())))

    #THROUGHPOUT
    y_Axis_Thresh = [sum([pair[1] for pair in thrs])  for _ , thrs in threshold_throughput_avg.items()]  #sum throughput reported by each client
    y_Axis_Form = [sum([pair[1] for pair in thrs])   for _ , thrs in form_throughput_avg.items()] #thrs is tuple (client id, throughput)
    y_Axis_Gen = [sum([pair[1] for pair in thrs])   for _ , thrs in generalized_throughput_avg.items()] #thrs is tuple (client id, throughput)
    y_Axis_Mix = [sum([pair[1] for pair in thrs])  for _ , thrs in mix_throughput_avg.items()]  #sum throughput reported by each client
   
    line_Thresh, = ax1.plot(x_Axis, y_Axis_Thresh, label=offLabel, color='C0', marker='*',markersize=7)
    line_Form, = ax1.plot(x_Axis, y_Axis_Form, label=formLabel, color='C3', marker='1')
    line_Gen, = ax1.plot(x_Axis, y_Axis_Gen, label=onLabel, color='C1', marker='3')
    line_Mix, = ax1.plot(x_Axis, y_Axis_Mix, label=mixLabel, color='C2', marker='x', markersize=7)
   
    ax1.set_ylabel("Throughput (Kops/sec)")
    ax1.set_ylim(bottom=0)
    #ax1.legend(handles=[line_Thresh, line_Form, line_Gen, line_Mix], labels=[offLabel, formLabel, onLabel, mixLabel], loc='upper left')
    
    #LATENCY
    y_Axis_Thresh = [sum([pair[1] for pair in lats]) / len(lats) for _ , lats in threshold_latency.items()] #avg all clients and all runs (:=responses from same client)
    y_Axis_Form = [sum([pair[1] for pair in lats]) / len(lats) for _ , lats in form_latency.items()]
    y_Axis_Gen = [sum([pair[1] for pair in lats]) / len(lats) for _ , lats in generalized_latency.items()] #lats is tuple (client id, latency)
    y_Axis_Mix= [sum([pair[1] for pair in lats]) / len(lats) for _ , lats in mix_latency.items()]
    
    line_Thresh_lat, = ax2.plot(x_Axis, y_Axis_Thresh,linestyle='dashed', label=offLabel, color='C0', marker='*', markersize=7)
    line_Form_lat, = ax2.plot(x_Axis, y_Axis_Form, linestyle='dashed', label=formLabel, color='C3', marker='1')
    line_Gen_lat, = ax2.plot(x_Axis, y_Axis_Gen, linestyle='dashed', label=onLabel, color='C1', marker='3')
    line_Mix_lat, = ax2.plot(x_Axis, y_Axis_Mix, linestyle='dashed', label=mixLabel, color='C2', marker='x', markersize=7)
    
    ax2.set_ylabel("Latency (msec)")
    ax2.set_ylim(bottom=0)
    #ax2.legend(handles=[line_Thresh, line_Form, line_Gen, line_Mix], labels=[offLabel, formLabel, onLabel, mixLabel], loc='upper right')
    
    #LEGEND
    # solid_patch = mpatches.Patch(color='black', marker="-", label='Throughput')
    # dashed_patch = mpatches.Patch(color='black', marker="--", label='Latency')   
    solid_patch = mlines.Line2D([], [], color='black', marker=None, label='Throughput')
    dashed_patch = mlines.Line2D([], [], color='black', marker=None, linestyle='dashed', label='Latency')
    first_legend = plt.legend(handles=[line_Thresh, line_Form, line_Gen, line_Mix], labels=[offLabel, formLabel, onLabel, mixLabel], loc='upper center')
    plt.gca().add_artist(first_legend)
    plt.legend(handles=[solid_patch, dashed_patch], labels=["Throughput", "Latency"], loc='upper right')
    
    plt.savefig("plots/scal.png")
    

    # print([sum([pair[1] for pair in thrs])  for _ , thrs in threshold_throughput_avg.items()] )
    # print([sum([pair[1] for pair in thrs])   for _ , thrs in form_throughput_avg.items()] )
    # print([sum([pair[1] for pair in thrs])   for _ , thrs in generalized_throughput_avg.items()])
    # print([sum([pair[1] for pair in thrs])  for _ , thrs in mix_throughput_avg.items()] )
    # print([sum([pair[1] for pair in lats]) / len(lats) for _ , lats in threshold_latency.items()])
    # print([sum([pair[1] for pair in lats]) / len(lats) for _ , lats in form_latency.items()])
    # print([sum([pair[1] for pair in lats]) / len(lats) for _ , lats in generalized_latency.items()])
    # print([sum([pair[1] for pair in lats]) / len(lats) for _ , lats in mix_latency.items()])
    

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