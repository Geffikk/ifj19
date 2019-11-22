cmake_minimum_required(VERSION 3.15)
project(IFJ_ACTUAL C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(IFJ_ACTUAL
        lexem_string.c
        lexem_string.h
        main.c
        reader.h
        scanner.c
        scanner.h
        stack.c
        stack.h
        )
