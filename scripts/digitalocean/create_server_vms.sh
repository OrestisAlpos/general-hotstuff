TAG=HOTSTUFF_AUTO_GEN_SERVER
# SIZE="s-4vcpu-8gb"
SIZE="c-16"

#image 09.04.21
#81935285
#image 11.05.20
#63468559
IMAGE="81935285"

DROPLET_COUNT=4

for d in $(seq 0 $(($DROPLET_COUNT / 10)) )
do
    NAMES=""
    bottom=$(($d * 10))
    top=$(($d * 10 + 9))
    last=$(($top < $DROPLET_COUNT - 1 ? $top : $DROPLET_COUNT - 1))
    for i in $(seq $bottom $last  )
    do
        NAMES+="\"hotstuff-$i\""
        if [ $i -lt $last ]
        then
            NAMES+=","
        fi
    done  
    curl -X POST \
    -H "Content-Type: application/json" \
    -H "Authorization: Bearer $TOKEN" \
    -d '{"names":['${NAMES}'],
        "region":"fra1",
        "size":"'$SIZE'",
        "image":'$IMAGE',
        "ssh_keys":["0e:e7:2b:2a:15:14:8b:56:0c:a9:b7:73:a4:ca:7a:9b"],
        "backups":false, 
        "ipv6":true,
        "user_data":null,
        "private_networking":null,
        "tags":["'${TAG}'"]}' \
    "https://api.digitalocean.com/v2/droplets" 
done