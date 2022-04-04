cmake_minimum_required(VERSION 3.19)
project(Project C)

set(CMAKE_C_STANDARD 11)

add_executable(Project main.c server.c server.h client.c client.h)