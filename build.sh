#!/bin/bash
if [ ! -d "bin" ]; then
    mkdir bin
else
	rm bin/*
fi
g++ -g -O0 -I . -o bin/interrupts interrupts_101299776_101187793.cpp
#g++ -std=c++17 interrupts.cpp -o bin/interrupts_sim
