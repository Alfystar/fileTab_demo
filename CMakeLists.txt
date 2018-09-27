cmake_minimum_required(VERSION 3.9)
set(CMAKE_C_STANDARD 11)

project(fileTab_demo)

add_executable(fileTab_demo main.c tableFile.c tableFile.h)
