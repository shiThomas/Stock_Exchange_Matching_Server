# !/bin/bash

#make clean
#make

master_hmn=vcm-9229.vm.duke.edu
master_port=12345

./ExchangeMatchingServer $master_port
#./ringmaster $master_port $players $hops &
#sleep 1

wait
