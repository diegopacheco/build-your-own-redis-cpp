#!/bin/bash

mkdir -p target/

g++ -Wall -Wextra -O2 -g common.h hashtable.cpp zset.cpp avl.cpp test_offset.cpp -o target/test_offset
g++ -Wall -Wextra -O2 -g common.h hashtable.cpp zset.cpp avl.cpp server.cpp -o target/server