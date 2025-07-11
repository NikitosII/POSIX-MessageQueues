#!/bin/bash

gcc file1.c -o sender -lrt -lpthread
if [ $? -ne 0 ]; then
    echo "Error: Failed to compile"
    exit 1
fi

./sender

rm -f sender

