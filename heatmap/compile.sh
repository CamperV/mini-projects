#!/bin/bash
g++ -std=c++0x `pkg-config --cflags opencv` -o impl impl.cpp heatmap.cpp -L/usr/local/include `pkg-config --libs opencv`
