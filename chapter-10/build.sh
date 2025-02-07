#!/bin/bash

mkdir -p target/

g++ -Wall -Wextra -O2 -g avl.cpp test_avl.cpp -o target/main
