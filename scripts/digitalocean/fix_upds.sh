CLIENTS=$(<conf/clients.txt)
SERVERS=$(<conf/nodeips.txt)
for CL in ${CLIENTS} ; do
    rsync -avz src/hotstuff_client.cpp root@${CL}:~/libhotstuff/src/ &
    rsync -avz src/hotstuff.cpp root@${CL}:~/libhotstuff/src/ &
    rsync -avz include/hotstuff/hotstuff.h root@${CL}:~/libhotstuff/include/hotstuff/ &
done
for SRV in ${SERVERS} ; do
    rsync -avz src/hotstuff.cpp root@${SRV}:~/libhotstuff/src/ &
    rsync -avz include/hotstuff/hotstuff.h root@${SRV}:~/libhotstuff/include/hotstuff/ &
done
wait