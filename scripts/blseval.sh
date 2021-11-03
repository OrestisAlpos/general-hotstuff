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
    if [ ${HS_VERSION} == "orig" ]; then
        HS_APP_EXEC="hotstuff-app_thr_dsa"
        HS_CLIENT_EXEC="hotstuff-client_thr_dsa"
        KEYGEN_EXEC="hotstuff-keygen_thr"
        KEYGEN_ALGO="secp256k1"
    elif [ ${HS_VERSION} == "gen" ]; then
        HS_APP_EXEC="hotstuff-app_gen_bls"
        HS_CLIENT_EXEC="hotstuff-client_gen_bls"
        KEYGEN_EXEC="hotstuff-keygen_gen"
        KEYGEN_ALGO="bls"
    fi
    for server_count in 4 #4 7 10 13 16 19 22 25 28 31
    do
        ac="a_c_${HS_VERSION}_${server_count}"
        an="a_n_${HS_VERSION}_${server_count}"
        rm -rf $an
        for sip in $server_ips ; do
            ssh -l "root" ${sip} "rm -rf ~/libhotstuff/$an" &
        done
        rm -rf $ac
        for cip in $client_ips ; do
            ssh -l "root" ${cip} "rm -rf ~/libhotstuff/$ac" &
        done
        wait
        echo "#### STARTING WITH ${server_count} REPLICAS, ${NUM_CLIENTS} CLIENTS, AND HS_VERSION = ${HS_VERSION} ####"
        python3 scripts/gen_conf.py --keygen "./${KEYGEN_EXEC}" --block-size ${BLOCK_SIZE} --pace-maker rr --algo "${KEYGEN_ALGO}" --quorums_file "conf/quorum_confs/quorums_thres_${server_count}.json" --usetls 0
        cp "conf/quorum_confs/quorums_thres_${server_count}.json" conf/quorums.json || die "conf/quorum_confs/quorums_thres_${server_count}.json NOT FOUND"
        echo "## Starting the replicas"
        scripts/run.sh --bin ${HS_APP_EXEC} start $an
        echo "## Give 15s for the system to start"
        sleep 15
        #------------
            # scripts/digitalocean/get_client_ips.sh
            # head -n $NUM_CLIENTS conf/clients.txt | sponge conf/clients.txt
            # client_ips=$(<conf/clients.txt)
        first=1
        for RATE in 4 5 6 7
        do
            RATE_PER_CLIENT=$(( ${RATE} * ${BLOCK_SIZE} / ${NUM_CLIENTS} ))
            echo "## STARTING ${NUM_CLIENTS} CLIENTS WITH RATE PER CLIENT ${RATE_PER_CLIENT}."
            if [ $first == 1 ]; then
                scripts/run_client.sh --bin ${HS_CLIENT_EXEC} --max-async ${RATE_PER_CLIENT} start $ac
                let first++
            else
                scripts/run_client.sh --bin ${HS_CLIENT_EXEC} --max-async ${RATE_PER_CLIENT} restart $ac
            fi
            echo "## Let the system run for some time"
            sleep ${RUN_TIME}
            echo "## Stopping the clients"
            scripts/run_client.sh stop $ac
            echo "## Give ${SLEEP_TIME}s for the clients to receive the responses and store them"
            sleep $SLEEP_TIME
        done 
        echo "## FINISHED. Fetching results"
        scnt=0
        for sip in $server_ips ; do
            ((scnt=scnt+1))
            if [ $scnt -gt $server_count ]; then
                break
            fi
            rsync -avz root@${sip}:~/libhotstuff/$an/ $an/ > /dev/null &
        done  
        for cip in $client_ips ; do
            rsync -avz root@${cip}:~/libhotstuff/$ac/ $ac/ > /dev/null &
        done
        wait

        # rm "plots/${server_count}.png"
        # for i in $(seq 0 $(($NUM_CLIENTS - 1)))
        # do
        #     ret=$(cat $ac/$i/log | python3 scripts/thr_hist.py --plot --output "plots/${server_count}.png")
        #     echo "${HS_VERSION}.$server_count.thr.$i. $(grep 'thr_wout_outliers' <<< "${ret}" | grep -Eo '[0-9]+[.][0-9]+')" >> res/res_bls_scal.txt
        #     echo "${HS_VERSION}.$server_count.lat.$i. $(grep 'lat_wout_outliers' <<< "${ret}" | grep -Eo '[0-9]+[.][0-9]+')" >> res/res_bls_scal.txt
        # done

        # done
        echo "## STOPPING THE REPLICAS"
        scripts/run.sh stop $an     
    done
done

# python3 scripts/plot_scal.py 

