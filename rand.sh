#!/bin/bash

for i in $(seq $1 $2); do
       	./node examples/random $i | tee out-$i &
done
