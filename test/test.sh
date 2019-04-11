# !/bin/bash

master_hmn=localhost
master_port=12345
#players=10
#hops=100

#./ringmaster $master_port $players $hops &
#sleep 1

date >> date.txt
./client $master_hmn $master_port create1.xml &

<<<<<<< HEAD
for ((i=0;i<2;i++))
=======
for ((i=0;i<400;i++))
>>>>>>> d756a147cefb4e85937c8595cc267413d9edcf57
do
    ./client $master_hmn $master_port trans1.xml &
    # ./client $master_hmn $master_port trans1.xml &
    # ./client $master_hmn $master_port trans2.xml &
    # ./client $master_hmn $master_port trans3.xml &
    # ./client $master_hmn $master_port trans4.xml &
    # ./client $master_hmn $master_port trans5.xml &
    # ./client $master_hmn $master_port trans6.xml &
    # ./client $master_hmn $master_port trans7.xml &
done
wait
date >> date.txt

exit 0
