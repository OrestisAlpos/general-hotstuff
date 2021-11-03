SCRIPT="cd ~/gmp-6.1.2; ./configure; make; make install; cd ~/ntl-11.4.3/src; ./configure; make; make install; cd ~/libhotstuff; make"
REMOTE_SERVERS=$(<conf/replicas_exter.txt)
for HOST in ${REMOTE_SERVERS} ; do
    echo "#### REINSTALLING LIBS ${HOST} ####"
    ssh -l "root" ${HOST} "${SCRIPT}" &
done

REMOTE_CLIENTS=$(<conf/clients.txt)
for HOST in ${REMOTE_CLIENTS} ; do
    echo "#### REINSTALLING LIBS ${HOST} ####"
    ssh -l "root" ${HOST} "${SCRIPT}" &
done