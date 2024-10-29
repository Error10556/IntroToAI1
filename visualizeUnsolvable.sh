#!/bin/sh
make -s testVisualizer.bin
tests=$(./searchUnsolvable.sh "tests/*" | sort)
for test in $tests; do
    echo $test
    ./testVisualizer.bin $test
done
