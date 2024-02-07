#!/bin/bash


set -eux
gcc -c  ./glad/src/glad.c -o build/glad.o    -I./glad/include/  
g++  main.cpp ./jsoncpp-1.9.5/lib_json/*.cpp     build/glad.o      -o ./build/main `pkg-config --cflags --libs glfw3` `pkg-config --cflags --libs opengl`  -I./glad/include/  -I./jsoncpp-1.9.5/include/ -I./jsoncpp-1.9.5/include/json/
./build/main