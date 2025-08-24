#!/bin/zsh

cd ./

sudo clang++ -std=c++23 -I./src/include -L/usr/lib src/functions.cpp src/main.cpp -lncurses -o /usr/bin/pebtxt

# sym link with /usr/bin/
#ln -s /usr/bin/pebtxt ./build/pebtxt
