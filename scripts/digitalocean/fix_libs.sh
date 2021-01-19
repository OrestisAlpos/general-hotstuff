SCRIPT="cd ~/gmp-6.1.2; ./configure; make; make install; cd ~/ntl-11.4.3/src; ./configure; make; make install; cd ~/libhotstuff; make"
HOSTS="127.0.0.1"
for HOST in ${HOSTS} ; do
    echo "#### REINSTALLING LIBS ${HOST} ####"
    ssh -l "root" ${HOST} "${SCRIPT}"
done