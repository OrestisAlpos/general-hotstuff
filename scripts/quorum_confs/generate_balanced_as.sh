create_replicas_string(){
    conf=""
    for i in $(seq $1 $(($2 - 1)) )
    do
        conf+="$i"
        if [ $i -lt $(($2-1)) ]
        then
            conf+=","
        fi
    done
    echo $conf
}

for replicas in $(seq 3 103)
do
    # or
    conf="{\"select\": 1, \"out-of\": ["
    nominator=$(($replicas + 1)) 
    denominator=2
    t=$(( ($nominator+$denominator-1) / $denominator )) #get t = ceiling of (n+1)/2
    nominator=$t 
    # denominator=2
    # t=$(( ($nominator+$denominator-1) / $denominator )) #get ceiling of (n+1)/2
    k=$(( ($replicas - $t) / 2  ))
    if [ $k -eq 0 ]
    then
        k=1
    fi
    # t out-of P
    conf+="{\"select\": $t, \"out-of\": ["
    conf+=$( create_replicas_string 0 $replicas )
    conf+="]},"
    # and
    conf+="{\"select\": 2, \"out-of\": ["
    # k of P1
    conf+="{\"select\": $k, \"out-of\": ["
    conf+=$( create_replicas_string 0 $t )
    conf+="]},"
    # k of P2
    conf+="{\"select\": $k, \"out-of\": ["
    conf+=$( create_replicas_string $t $replicas )
    conf+="]}"

    conf+="]}"
    conf+="]}"
    echo $conf > conf/quorum_confs/unbalanced_${replicas}.json
done

