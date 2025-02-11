#!/bin/bash

mkdir -p target/

g++ -Wall -Wextra -O2 -g common.h hashtable.cpp heap.cpp zset.cpp avl.cpp thread_pool.cpp server.cpp -o target/server
g++ -Wall -Wextra -O2 -g common.h hashtable.cpp heap.cpp zset.cpp avl.cpp client.cpp -o target/client