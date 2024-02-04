#!/bin/bash


set -eux
gcc -c  ./glad/src/glad.c -o build/glad.o    -I./glad/include/  
g++ -c  main.cpp          -o ./build/main.o  -I./glad/include/ 
g++ -o build/main build/glad.o build/main.o  `pkg-config --cflags --libs glfw3` `pkg-config --cflags --libs opengl`
./build/main