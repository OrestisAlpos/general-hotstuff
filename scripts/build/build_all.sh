#!/bin/bash

# MSP="ON"
# CL=$1
# SRV=$1
# if [ $1 = "MIX" ] || [ $1 = "TMIX" ]; then
#     CL="OFF"
#     SRV="ON"
# fi
# if [ $1 = "FORM" ] || [ $1 = "TFORM" ]; then
#     CL="ON"
#     SRV="ON"
#     MSP="OFF"
# fi

REMOTE_SERVERS=$(<conf/replicas_exter.txt)
# SERVER_SCRIPT="cd libhotstuff; cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED=OFF -DHOTSTUFF_PROTO_LOG=ON -DUSE_GENERALIZED_QUORUMS=$SRV -DGENERALIZED_QUORUMS_WITH_MSP=$MSP -DHOTSTUFF_ENABLE_BENCHMARK=ON; make"
REMOTE_CLIENTS=$(<conf/clients.txt)
# CLIENT_SCRIPT="cd libhotstuff; cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED=OFF -DHOTSTUFF_PROTO_LOG=ON -DUSE_GENERALIZED_QUORUMS=$CL -DGENERALIZED_QUORUMS_WITH_MSP=$MSP -DHOTSTUFF_ENABLE_BENCHMARK=ON; make"

rm -f scripts/build/res/*

echo "Starting Building"
for SRV in ${REMOTE_SERVERS} ; do
    # ssh -l "root" ${SRV} "${SERVER_SCRIPT}" &> "scripts/build/res/${SRV}.txt" &
    ssh root@${SRV} 'cd /root/libhotstuff; bash -s' < scripts/build/build_local.sh &> "scripts/build/res/${SRV}.txt" &
done
for CLIENT in ${REMOTE_CLIENTS} ; do
    # ssh -l "root" ${CLIENT} "${CLIENT_SCRIPT}" &> "scripts/build/res/${CLIENT}.txt" &
    ssh root@${CLIENT} 'cd /root/libhotstuff; bash -s' < scripts/build/build_local.sh &> "scripts/build/res/${CLIENT}.txt" &
done
wait

echo "All built"
