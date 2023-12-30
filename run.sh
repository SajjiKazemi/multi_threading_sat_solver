#!/bin/bash

t = 10

output = "output.txt"

for ((i=5; i<=50; i+=5))
do
    echo "Running for $i"
    /home/agurfink/ece650/graphGen/graphGen "$i"  >> "$output_file"

done