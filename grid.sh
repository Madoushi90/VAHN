#!/bin/bash

for i in $(seq $1 $2); do
	for j in $(seq 1 4); do
		./node examples/grid $i$j 2>&1 | tee out-$i$j &
	done
done

