cmake_minimum_required(VERSION 3.20)
project(elevator C)

set(CMAKE_C_STANDARD 99)

add_executable(elevator main.c simulateFunc.h BASICSTRUCT.h utils.h)
