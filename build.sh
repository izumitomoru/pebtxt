#!/bin/zsh

cd /run/media/pebarch/pebdrive/_Code/C++/filetests/

rm /run/media/pebarch/pebdrive/_Code/C++/filetests/build/build.out

# compile manually
clang++ -Wall -g -std=c++23 -I./src/include src/main.cpp src/functions.cpp -o build/build.out

# compile automatically
#clang++ -std=c++23 -I./src/include $(find src/ -type f -iregex ".*\.cpp") -o build/build.out

# run
build/build.out

# build object files then compile
#clang++ -c -Wall src/main.cpp src/log.cpp -o build/main.o build/log.o
#clang++ -c -Wall src/log.cpp -o log.o
#clang++ -Wall build/main.o build/log.o -o build/test.out
