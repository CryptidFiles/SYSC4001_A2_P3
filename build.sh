#!/bin/bash
if [ ! -d "bin" ]; then
    mkdir bin
else
	rm bin/*
fi
g++ -g -O0 -I . -o bin/interrupts interrupts.cpp
#g++ -std=c++17 interrupts.cpp -o bin/interrupts_sim
