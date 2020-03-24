RUN_TIME=60

declare -a throughput_vals
declare -a latency_vals
for server_count in 4 5 6 7 8
do
    echo "#### STARTING WITH ${server_count} REPLICAS ####"
    echo "## Generating conf files"
    python3 scripts/gen_conf.py --block-size 400 --iter ${server_count} --pace-maker dummy
    echo "## Generating quorums.json file"
    cp "quorum_confs/quorums_thres_${server_count}.json" quorums.json
    echo "## Starting replicas"
    rm -rf a_n && scripts/run.sh start a_n
    echo "## Starting clients"
    rm -rf a_c && scripts/run_client.sh start a_c
    echo "## Let's let the system run for some time"
    sleep ${RUN_TIME}
    echo "## Stopping the system"
    scripts/run.sh stop a_n
    scripts/run_client.sh stop a_c

    sleep 10
    
    rm "plots/${server_count}.png"
    ret=$(cat a_c/0/log | python3 scripts/thr_hist.py --plot --output "plots/${server_count}.png")
    throughput_vals+=$(grep 'thr_wout_outliers' <<< "${ret}" | grep -Eo '[0-9]+[.][0-9]+')
    latency_vals+=$(grep 'lat_wout_outliers' <<< "${ret}" | grep -Eo '[0-9]+[.][0-9]+')
done

for i in "${throughput_vals[@]}"; do echo "$i"; done
for i in "${latency_vals[@]}"; do echo "$i"; done
