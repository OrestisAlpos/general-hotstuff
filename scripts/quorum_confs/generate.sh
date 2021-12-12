for replicas in $(seq 3 103)
do
    divide=$((2 * $replicas + 1))
    by=3
    result=$(( ($divide+$by-1) / $by ))  #get ceiling of (2k+1)/3
    conf="{\"select\": $result, \"out-of\": ["
    for i in $(seq 0 $((replicas-1)))
    do
        conf+="$i"
        if [ $i -lt $((replicas-1)) ]
        then
            conf+=","
        fi
    done
    conf+="]}"
    echo $conf > conf/quorum_confs/quorums_thres_${replicas}.json
done

for replicas in $(seq 3 103)
do
    divide=$(($replicas + 1)) 
    by=2
    result=$(( ($divide+$by-1) / $by )) #get ceiling of (k+1)/2
    conf="{\"select\": $result, \"out-of\": ["
    for i in $(seq 0 $((replicas-1)))
    do
        conf+="$i"
        if [ $i -lt $((replicas-1)) ]
        then
            conf+=","
        fi
    done
    conf+="]}"
    echo $conf > conf/quorum_confs/quorums_maj_thres_${replicas}.json
done

for k in $(seq 4 13)
do
    divide=$((2 * $k + 1)) #get ceiling of (2k+1)/3
    by=3
    result=$(( ($divide+$by-1) / $by ))
    conf="{\"select\": $result, \"out-of\": ["
    for l in $(seq 0 $((k-1)))
    do
        conf_2="{\"select\": 2, \"out-of\": ["
        conf_2+="$l"
        conf_2+=", {\"select\": 2, \"out-of\": ["
        for m in $(seq $((k + 3*l)) $((k + 3*l + 3)))
        do
            if [ $m -lt $((4*k)) ]; then
                conf_2+="$m"
            else
                conf_2+="$k"
            fi
            if [ $m -lt $((k + 3*l + 3)) ]; then
                conf_2+=","
            fi
        done
        conf_2+="]} ]}"
        if [ $l -lt $((k-1)) ]; then
            conf_2+=","
        fi
        conf="$conf"$'\n'"    $conf_2"
    done
    conf="$conf"$'\n'"]}"
    echo "$conf" > conf/quorum_confs/1common_k${k}.json
done

