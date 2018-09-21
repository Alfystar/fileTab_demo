cmake_minimum_required(VERSION 3.9)
project(fileTab_demo C)

set(CMAKE_C_STANDARD 11)

add_executable(fileTab_demo main.c tableFile.c tableFile.h)