import argparse
import math
import sys

# 0  1  2  3 
# 4  5  6  7 
# 8  9 10 11
#12 13 14 15 

# Return all paries in row i. i in [1,k]
def parties_in_row(i, k):
    if i > k:
        raise ValueError('i')
    parties_in_row = [p for p in range((i - 1)*k, i*k)]
    res = "".join(str(p) + "," for p in parties_in_row)
    return res[:-1]

# Return all paries in col j. j in [1,k]
def parties_in_col(j, k):
    if j > k:
        raise ValueError('j')
    parties_in_col = [c*k + j - 1 for c in range(k)]
    res = "".join(str(p) + "," for p in parties_in_col)
    return res[:-1]

def get_conf(n):
    k = math.floor(math.sqrt(n))
    b = k - 1
    t = math.ceil(math.sqrt(b / 2 + 1))
    k_rows = ""
    for i in range(1, k + 1):
        k_rows += "{{\"select\": {k}, \"out-of\": [{row_i}]}}".format(k=k, row_i=parties_in_row(i, k))
        if i != k:
            k_rows +=","
    k_cols = ""
    for j in range(1, k + 1):
        k_cols += "{{\"select\": {k}, \"out-of\": [{col_j}]}}".format(k=k, col_j=parties_in_col(j, k))
        if j != k:
            k_cols +=","
    theta_1 = "{{\"select\": {t}, \"out-of\": [{k_rows}]}}".format(t=t, k_rows = k_rows)
    theta_2 = "{{\"select\": {t}, \"out-of\": [{k_cols}]}}".format(t=t, k_cols=k_cols)
    conf = "{{\"select\": 2, \"out-of\":[{theta_1}, {theta_2}]}}".format(theta_1=theta_1, theta_2=theta_2)
    return conf;


if __name__ == "__main__":
    for n in [4, 16, 36, 64, 100]:
        with open("conf/quorum_confs/kgrid_" + str(n) + ".json", "w") as fp:
            fp.write(get_conf(n))
