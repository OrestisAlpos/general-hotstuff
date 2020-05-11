import sys
import re
import argparse
import numpy as np
from datetime import datetime, timedelta

def remove_outliers(x, outlierConstant = 1.5):
    a = np.array(x)
    upper_quartile = np.percentile(a, 75)
    lower_quartile = np.percentile(a, 25)
    IQR = (upper_quartile - lower_quartile) * outlierConstant
    quartileSet = (lower_quartile - IQR, upper_quartile + IQR)
    resultList = []
    removedList = []
    for y in a.tolist():
        if y >= quartileSet[0] and y <= quartileSet[1]:
            resultList.append(y)
        else:
            removedList.append(y)
    return (resultList, removedList)

def str2datetime(s):
    parts = s.split('.')
    dt = datetime.strptime(parts[0], "%Y-%m-%d %H:%M:%S")
    return dt.replace(microsecond=int(parts[1]))


def plot_thr(fname):
    import matplotlib.pyplot as plt
    x = range(len(values))
    y = values
    plt.xlabel(r"time")
    plt.ylabel(r"tx/sec")
    plt.plot(x, y)
    plt.show()
    plt.savefig(fname)

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--interval', type=float, default=1, required=False)
    parser.add_argument('--output', type=str, default="hist.png", required=False)
    parser.add_argument('--plot', action='store_true')
    args = parser.parse_args()
    commit_pat = re.compile('([^[].*) \[hotstuff info\] ([0-9.]*)$')
    interval = args.interval
    begin_time = None
    next_begin_time = None
    cnt = 0
    lats = []
    timestamps = []
    values = []
    for line in sys.stdin:
        m = commit_pat.match(line)
        if m:
            timestamps.append(str2datetime(m.group(1)))
            lats.append(float(m.group(2)))
    timestamps.sort()
    for timestamp in timestamps:
        if begin_time and timestamp < next_begin_time:
            cnt += 1
        else:
            if begin_time:
                values.append(cnt)
            begin_time = timestamp
            next_begin_time = begin_time + timedelta(seconds=interval)
            cnt = 1
    values.append(cnt)
    print("thr_values = ", values)
    print("thr = {:.3f}Kops/sec".format(sum(values) / len(values) / 1e3 ))
    values_norm, _ = remove_outliers(values)
    print("thr_wout_outliers = {:.3f}Kops/sec".format(sum(values_norm) / len(values_norm) / 1e3))

    if len(lats) == 0:
        print("lat = {:.3f}".format(0))
        print("lat_wout_outliers = {:.3f}".format(0))
    else:
        print("lat = {:.3f}ms".format(sum(lats) / len(lats) * 1e3))
        lats, _ = remove_outliers(lats)
        print("lat_wout_outliers = {:.3f}ms".format(sum(lats) / len(lats) * 1e3))
    if args.plot:
        plot_thr(args.output)
