#!/bin/bash
g++ -std=c++0x `pkg-config --cflags opencv` -o mockup zone_mockup.cpp -L/usr/local/include `pkg-config --libs opencv`
