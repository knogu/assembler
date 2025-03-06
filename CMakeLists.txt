cmake_minimum_required(VERSION 3.29)
project(asm C)

set(CMAKE_C_STANDARD 11)

add_executable(asm
        main.c
        tokenize.c
        parse.c)
