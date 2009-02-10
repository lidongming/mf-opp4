#! /bin/sh

mv scalar.clean scalar.clean.back
mv omnetpp.sca omnetpp.sca.back
for i in  1 2 3 4 5 6 7 8 9 10 ; do
./csma -f omnetpp.ini -r $i
done
./cleanupall.sh
