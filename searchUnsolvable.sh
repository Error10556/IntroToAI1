#!/bin/bash
make -s shortestPathLength.bin
for file in $1
do
    len=$(./shortestPathLength.bin $file)
    if [ $len -eq -1 ]; then
        echo $file
    fi
done

