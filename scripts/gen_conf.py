import os, re
import subprocess
import itertools
import argparse

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Generate configuration file for a batch of replicas')
    parser.add_argument('--prefix', type=str, default='conf/hotstuff.gen')
    parser.add_argument('--ips_inter', type=str, default="conf/replicas_inter.txt")
    parser.add_argument('--ips_exter', type=str, default="conf/replicas_exter.txt")
    parser.add_argument('--iter', type=int, default=10)
    parser.add_argument('--pport', type=int, default=10000)
    parser.add_argument('--cport', type=int, default=20000)
    parser.add_argument('--keygen', type=str, default='./hotstuff-keygen')
    parser.add_argument('--tls-keygen', type=str, default='./hotstuff-tls-keygen')
    parser.add_argument('--nodes', type=str, default='conf/nodes.txt')
    parser.add_argument('--block-size', type=int, default=1)
    parser.add_argument('--pace-maker', type=str, default='dummy')
    parser.add_argument('--nworker', type=int, default=4)
    parser.add_argument('--repnworker', type=int, default=4)
    parser.add_argument('--clinworker', type=int, default=4)
    parser.add_argument('--repburst', type=int, default=1000)
    parser.add_argument('--cliburst', type=int, default=1000)
    parser.add_argument('--algo', type=str, default='secp256k1')
    parser.add_argument('--quorums_file', type=str, default='')
    parser.add_argument('--usetls', type=int, default=1)
    args = parser.parse_args()


    if args.ips_inter is None:
        ips_inter = ['127.0.0.1']
        ips_exter = ['127.0.0.1']
    else:
        ips_inter = [l.strip() for l in open(args.ips_inter, 'r').readlines()]
        ips_exter = [l.strip() for l in open(args.ips_exter, 'r').readlines()]
    prefix = args.prefix
    iter = args.iter
    base_pport = args.pport
    base_cport = args.cport
    keygen_bin = args.keygen
    tls_keygen_bin = args.tls_keygen

    if not os.path.isfile(args.quorums_file):
        raise Exception('File not found: '+args.quorums_file, 'pwd: '+os.getcwd())

    main_conf = open("{}.conf".format(prefix), 'w')
    nodes = open(args.nodes, 'w')
    port_count = {}
    replicas_inter = []
    replicas_exter = []
    for ip in zip(ips_inter, ips_exter):
        i = port_count.setdefault(ip[0], 0)
        port_count[ip[0]] += 1
        replicas_inter.append("{}:{};{}".format(ip[0], base_pport + i, base_cport + i))
        replicas_exter.append("{}:{};{}".format(ip[1], base_pport + i, base_cport + i))
    p = subprocess.Popen([keygen_bin, '--num', str(len(replicas_inter)), '--algo', args.algo, '--quorums_file', args.quorums_file],
                        stdout=subprocess.PIPE, stderr=open(os.devnull, 'w'))
    keygen_out = [[t[4:] for t in l.decode('ascii').split()] for l in p.stdout]
    if args.algo == 'bls':
        globalKey = keygen_out[0]
        keys = keygen_out[1:]
    else:
        keys = keygen_out 
    tls_p = subprocess.Popen([tls_keygen_bin, '--num', str(len(replicas_inter))],
                        stdout=subprocess.PIPE, stderr=open(os.devnull, 'w'))
    tls_keys = [[t[4:] for t in l.decode('ascii').split()] for l in tls_p.stdout]
    #cert, priv, pub
    if args.block_size is not None:
        main_conf.write("block-size = {}\n".format(args.block_size))
    if args.nworker is not None:
        main_conf.write("nworker = {}\n".format(args.nworker))
    if args.repnworker is not None:
        main_conf.write("repnworker = {}\n".format(args.repnworker))
    if args.clinworker is not None:
        main_conf.write("clinworker = {}\n".format(args.clinworker))
    if args.repburst is not None:
        main_conf.write("repburst = {}\n".format(args.repburst))
    if args.cliburst is not None:
        main_conf.write("cliburst = {}\n".format(args.cliburst))
    if not (args.pace_maker is None):
        main_conf.write("pace-maker = {}\n".format(args.pace_maker))
    if args.algo == 'bls':
        main_conf.write("global-pubkey = {}\n".format(globalKey[0]))
    if args.usetls != 1:
        main_conf.write("notls = true\n")
    for r in zip(replicas_inter, keys, tls_keys, itertools.count(0), replicas_exter):
        main_conf.write("replica = {}, {}, {}\n".format(r[0], r[1][0], r[2][2])) #r[1][0] is pubkey, r[1][1] is prevkey
        r_conf_name = "{}-sec{}.conf".format(prefix, r[3])
        nodes.write("{}:{}\t{}\n".format(r[3], r[4], r_conf_name))
        r_conf = open(r_conf_name, 'w')
        r_conf.write("privkey = {}\n".format(r[1][1]))
        r_conf.write("tls-privkey = {}\n".format(r[2][1]))
        r_conf.write("tls-cert = {}\n".format(r[2][0]))
        r_conf.write("idx = {}\n".format(r[3]))