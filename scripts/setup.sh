#!/bin/bash

originalpath=`pwd` 

# Use oneline from stackoverflow to retrieve script directory 
scriptdir="$( cd "$( dirname "$0" )" && pwd )" 

buildpath=$scriptdir/../build

echo "Make sure you actually build recently..."
cd "$buildpath"
make all

echo "Jump back to original path"
cd "$originalpath"

echo "Run setup binary"
$buildpath/Setup
