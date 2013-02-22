#!/bin/bash

if [[ "$1" == "" ]]
then
	echo "No args supplied! Run $0 -h for more info"
	exit 1
fi

# Provide help
if [[ "$1" == "-h" ]]
then
	echo "Format $0 ARG will execute SandPile till given parameter"
	exit 1
fi

path=.

# Use oneline from stackoverflow to retrieve script directory 
scriptdir="$( cd "$( dirname "$0" )" && pwd )" 

binary=${scriptdir}/../build/SandPile

# Start in parallel a number of shells with the given binary
run=$1
log=${path}/log${run}.txt
echo >> $log
d=`date`
echo "--------------------------------------------------------------------------------" >> $log
echo "Run ($d) (can be just plotting)"  >> $log
echo "--------------------------------------------------------------------------------" >> $log
echo >> $log
gnome-terminal -x sh -c "$binary --run $run | tee -ai $log"


