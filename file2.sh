#!/bin/bash

gcc file2.c -o receiver -lrt -lpthread
if [ $? -ne 0 ]; then
    echo "Error: Failed to compile"
    exit 1
fi

./receiver

rm -f receiver
