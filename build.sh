#!/bin/zsh

#cd /run/media/pebarch/pebdrive/_Code/C++/filetests/
cd ./

#rm /run/media/pebarch/pebdrive/_Code/C++/filetests/build/build.out
#rm ./build/build.out
rm ./build/pebtxt

# compile manually
# as a future note, -l has to be AFTER the .cpp files for some fucking reason, and i figured that out myself because nowhere online told me so
# i'm gonna have a fucking aneurysm. screen -> dom -> component doesn't work, it has to be screen -> component -> dom
# honestly i really just do not understand linker syntax at all but whatever

#clang++ -Wall -g -std=c++23 -I./src/include -L/usr/lib src/functions.cpp src/main.cpp -lncurses -lftxui-screen -lftxui-dom -lftxui-component -lftxui-dom -lftxui-screen -o build/build.out
# no debug
#clang++ -std=c++23 -I./src/include -L/usr/lib src/functions.cpp src/main.cpp -lncurses -lftxui-screen -lftxui-dom -lftxui-component -lftxui-dom -lftxui-screen -o build/build.out
clang++ -std=c++23 -I./src/include -L/usr/lib src/functions.cpp src/main.cpp -lncurses -lftxui-screen -lftxui-dom -lftxui-component -lftxui-dom -lftxui-screen -o build/pebtext

# compile automatically
#clang++ -std=c++23 -I./src/include $(find src/ -type f -iregex ".*\.cpp") -o build/build.out

# run
#build/build.out

# build object files then compile
#clang++ -c -Wall src/main.cpp src/log.cpp -o build/main.o build/log.o
#clang++ -c -Wall src/log.cpp -o log.o
#clang++ -Wall build/main.o build/log.o -o build/test.out
