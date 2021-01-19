for replicas in 4 7 10 13 16 19 22 25 28 31 16 20 24 28 32 36 40 44 48 52
do
    divide=$((2 * $replicas + 1)) #get ceiling of (2k+1)/3
    by=3
    result=$(( ($divide+$by-1) / $by ))
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
    echo $conf > quorum_confs/quorums_thres_${replicas}.json
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
    echo "$conf" > quorum_confs/1common_k${k}.json
done

