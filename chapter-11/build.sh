#!/bin/bash

mkdir -p target/

g++ -Wall -Wextra -O2 -g zset.cpp avl.cpp test_offset.cpp -o target/test_offset
