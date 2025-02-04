#!/bin/bash

mkdir -p target/

g++ -Wall -Wextra -O2 -g server.cpp -o target/server
g++ -Wall -Wextra -O2 -g client.cpp -o target/client
