cmake_minimum_required(VERSION 3.15)
project(IFJ C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(IFJ
        lexem_string.c
        lexem_string.h
        main.c
        scanner.c
        scanner.h
        stack.c stack.h)
