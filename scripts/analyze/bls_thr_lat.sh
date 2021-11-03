#!/bin/bash
BLOCK_SIZE=400
RUN_TIME=60
SLEEP_TIME=15

# scripts/digitalocean/get_server_ips.sh
# scripts/digitalocean/get_client_ips.sh
# scripts/digitalocean/fix_upds.sh
# echo "#### BUILDING ALL SERVERS AND THE CLIENT"
# eval "scripts/build/build_all.sh $USE_GEN_QUORUMS"

NUM_CLIENTS=$(wc -l < conf/clients.txt) || die "conf/clients.txt NOT FOUND"
client_ips=$(<conf/clients.txt)
server_ips=$(<conf/replicas_exter.txt)

for HS_VERSION in "orig" "gen"
do
    for server_count in 4 #4 7 10 13 16 19 22 25 28 31
    do
        echo "## Analyzing experiment ${HS_VERSION}_${server_count}"
        ac="a_c_${HS_VERSION}_${server_count}"
        for i in $(seq 0 $(($NUM_CLIENTS - 1)))
        do
            echo "Using results of client $i"
            csplit --quiet --prefix=$ac/$i/log_part $ac/$i/log /max_async_num/ '{*}'
            parts=$( find . -regextype posix-extended -regex "./$ac/$i/log_part0[1-9]" )
            for part in $parts
            do
                rate=$( grep max_async_num $part | cut -d' ' -f"13" )
                echo "## Found part $part with rate $rate"
                ret=$(cat $part | python3 scripts/thr_hist.py --plot --output "$ac/$i/plot.png")
                echo "${HS_VERSION}.${server_count}.${rate}.thr_lat.$i. $(grep 'thr_wout_outliers' <<< "${ret}" | grep -Eo '[0-9]+[.][0-9]+') $(grep 'lat_wout_outliers' <<< "${ret}" | grep -Eo '[0-9]+[.][0-9]+') " >> res/res_bls_thr_lat.txt
                # echo "${HS_VERSION}.${server_count}.${rate}.lat.$i. $(grep 'lat_wout_outliers' <<< "${ret}" | grep -Eo '[0-9]+[.][0-9]+')" >> res/res_bls_thr_lat.txt
            done
            
        done
    done
done

# python3 scripts/plot_scal.py 

