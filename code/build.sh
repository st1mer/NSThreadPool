#!/bin/bash

g++ -Wall -g -shared -fPIC -std=c++0x -pthread -o libnsthreadpool.so NSThreadPool.cpp