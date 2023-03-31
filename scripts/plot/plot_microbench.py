import sys
import argparse
import numpy as np
from collections import defaultdict
import matplotlib.pyplot as plt
import matplotlib.lines as mlines

from plot_settings import *

def plot_n():
   
    plt.grid(True)
    plt.xlabel("Number of replicas")
    x_Axis = list(map(int, generalized_mem.keys()))     
    plt.xticks(list(map(int, generalized_mem.keys())))

    y_Axis_Thresh_mem = [v  * 0.001 for v in threshold_mem.values()]
    y_Axis_Form_mem = [v  * 0.001 for v in formula_mem.values()]
    y_Axis_Gen_mem = [v  * 0.001 for v in generalized_mem.values()]
    y_Axis_Thresh_1C_mem = [v  * 0.001 for v in threshold_mem_1C.values()]
    y_Axis_Form_1C_mem = [v  * 0.001 for v in formula_mem_1C.values()]
    y_Axis_Gen_1C_mem = [v  * 0.001 for v in generalized_mem_1C.values()]
   
    # # line_Thresh = ax1.plot(x_Axis, y_Axis_Thresh_mem, label=offLabel, color='C0', marker='*', lw=1)
    # line_Form = ax1.plot(x_Axis, y_Axis_Form_mem, label=formLabel, color='C3', marker='1', lw=1)
    # line_Gen = ax1.plot(x_Axis, y_Axis_Gen_mem, label=onLabel, color='C1', marker='3', lw=1)
    
    # line_Form1C, = plt.plot(x_Axis, y_Axis_Form_1C_mem, label=formLabel+", 2L1C BQS", color=formColor, marker='2')
    # line_FormSimpleQS, = plt.plot(x_Axis, y_Axis_Form_mem, label=formLabel+", threshold BQS", color=formColor, linestyle='dashed', marker='1')
    # line_Mix1C, = plt.plot(x_Axis, y_Axis_Gen_1C_mem, label=mixLabel+", 2L1C BQS", color=mixColor, marker='+')
    # line_MixSimpleQS, = plt.plot(x_Axis, y_Axis_Gen_mem, label=mixLabel+", threshold BQS", color=mixColor, linestyle='dashed', marker='x', markersize=7)

   
    line_Form1C, = plt.plot(x_Axis, y_Axis_Form_1C_mem, label="MBF", color=formColor, marker='2')
    line_Mix1C, = plt.plot(x_Axis, y_Axis_Gen_1C_mem, label="MSP", color=mixColor, marker='+')

    plt.ylabel("Memory (KB)")
    plt.ylim(bottom=0)
    plt.legend(loc='upper left')
    plt.savefig("plots/microbench_mem.png")
    plt.clf()

    plt.grid(True)
    plt.xlabel("Number of replicas")
    x_Axis = list(map(int, generalized_mem.keys()))     
    plt.xticks(list(map(int, generalized_mem.keys())))

    y_Axis_Thresh_time = list(threshold_time.values())
    y_Axis_Form_time = list(formula_time.values())
    y_Axis_Gen_time = list(generalized_time.values())
    y_Axis_Thresh_1C_time = list(threshold_time_1C.values())
    y_Axis_Form_1C_time = list(formula_time_1C.values())
    y_Axis_Gen_1C_time = list(generalized_time_1C.values())
   
    # # line_Thresh = ax2.plot(x_Axis, y_Axis_Thresh_time, linestyle='dashed', label=offLabel, color='C0', marker='*', lw=1)
    # line_Form = ax2.plot(x_Axis, y_Axis_Form_time, linestyle='dashed', label=formLabel, color='C3', marker='1', lw=1)
    # line_Gen = ax2.plot(x_Axis, y_Axis_Gen_time, linestyle='dashed', label=onLabel, color='C1', marker='3', lw=1)
    
    # line_Form1C_time, = plt.plot(x_Axis, y_Axis_Form_1C_time, label=formLabel+", 2L1C BQS", color=formColor, marker='2')
    # line_FormSimpleQS_time, = plt.plot(x_Axis, y_Axis_Form_time, label=formLabel+", threshold BQS", color=formColor, linestyle='dashed', marker='1')
    # line_Mix1C_time, = plt.plot(x_Axis, y_Axis_Gen_1C_time, label=mixLabel+", 2L1C BQS", color=mixColor, marker='+')
    # line_MixSimpleQS_time, = plt.plot(x_Axis, y_Axis_Gen_time, label=mixLabel+", threshold BQS", color=mixColor, linestyle='dashed', marker='x', markersize=7)

    line_Mix1C_time, = plt.plot(x_Axis, y_Axis_Gen_1C_time, label="MSP", color=mixColor, marker='+')
    line_Form1C_time, = plt.plot(x_Axis, y_Axis_Form_1C_time, label="MBF", color=formColor, marker='2')
    
    plt.ylabel("Time (μsec)")
    plt.ylim(bottom=0)
    plt.legend(loc='upper left')

    plt.savefig("plots/microbench_time.png")
    plt.clf()

    
if __name__ == '__main__':
    with open('res/res_microbench_1common_simple.txt') as fp:
        generalized_mem = dict()
        threshold_mem = dict()
        formula_mem = dict()
        generalized_time = dict()
        threshold_time = dict()
        formula_time = dict()
        for _, line in enumerate(fp):
            if line.startswith('#'):
                continue
            if line.startswith("ON"):
                sp = line.split('.')
                n = sp[1] #num of servers
                val = float(line.split()[1])
                if sp[2] == "mem":
                    generalized_mem[n] = val
                elif sp[2] == "time":
                    generalized_time[n] = val
            elif line.startswith("OFF"):
                sp = line.split('.')
                n = sp[1]
                val = float(line.split()[1])
                if sp[2] == "mem":
                    threshold_mem[n] = val
                elif sp[2] == "time":
                    threshold_time[n] = val
            elif line.startswith("FORM"):
                sp = line.split('.')
                n = sp[1]
                val = float(line.split()[1])
                if sp[2] == "mem":
                    formula_mem[n] = val
                elif sp[2] == "time":
                    formula_time[n] = val
    with open('res/res_microbench_1common.txt') as fp:
        generalized_mem_1C = dict()
        threshold_mem_1C = dict()
        formula_mem_1C = dict()
        generalized_time_1C = dict()
        threshold_time_1C = dict()
        formula_time_1C = dict()
        for _, line in enumerate(fp):
            if line.startswith('#'):
                continue
            if line.startswith("ON"):
                sp = line.split('.')
                n = sp[1] #num of servers
                val = float(line.split()[1])
                if sp[2] == "mem":
                    generalized_mem_1C[n] = val
                elif sp[2] == "time":
                    generalized_time_1C[n] = val
            elif line.startswith("OFF"):
                sp = line.split('.')
                n = sp[1]
                val = float(line.split()[1])
                if sp[2] == "mem":
                    threshold_mem_1C[n] = val
                elif sp[2] == "time":
                    threshold_time_1C[n] = val
            elif line.startswith("FORM"):
                sp = line.split('.')
                n = sp[1]
                val = float(line.split()[1])
                if sp[2] == "mem":
                    formula_mem_1C[n] = val
                elif sp[2] == "time":
                    formula_time_1C[n] = val
    # print('generalized_mem=', generalized_mem)
    # print('threshold_mem=', threshold_mem)
    # print('formula_mem=',formula_mem)
    # print('generalized_time=', generalized_time)
    # print('threshold_time=', threshold_time)
    # print('formula_time=',formula_time)
    plot_n()