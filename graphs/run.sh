#!/bin/bash

output = "./output.txt"


for ((i=5; i<=50; i+=5))
do
	echo "Running for $i"
	for ((t=0; t<10; t++))
	do
    
    	/home/agurfink/ece650/graphGen/graphGen "$i"  >>  ./output.txt
	echo ""  >>  ./output.txt
	done
done
