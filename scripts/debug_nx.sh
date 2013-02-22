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

run=$1
log=${path}/log${run}.txt

echo "run" > debug_cmds
echo "bt" >> debug_cmds
echo "quit" >> debug_cmds

gdb -x debug_cmds --args $binary --run $run 


