cmake_minimum_required(VERSION 3.27)
set(CMAKE_C_COMPILER "/usr/bin/gcc")

project(GraphicsEngine C)

set(CMAKE_C_STANDARD 11)

add_executable(GraphicsEngine main.c
        graphi.c
        test.c)
