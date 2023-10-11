#!/bin/bash

# Check if a filename was provided
#if [ "$#" -ne 1 ]; then
#    echo "Usage: $0 <mode>"
#    exit 1
#fi

A1 1 1 > out1 2>&1
A1 2 16 > out2 2>&1

filename1='out1'
filename2='out2'

tail -n +2 "$filename1" | sort > result1.out
tail -n +2 "$filename2" | sort > result2.out
