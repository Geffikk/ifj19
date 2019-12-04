cmake_minimum_required(VERSION 3.15)
project(IFJ_2019)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(IFJ_2019
        cmake-build-debug/cmake_install.cmake
        cmake-build-debug/CMakeCache.txt
        cmake-build-debug/IFJ_2019
        cmake-build-debug/IFJ_2019.cbp
        cmake-build-debug/Makefile
        analyza.c
        analyza.h
        CMakeLists.txt
        error.h
        expression.c
        expression.h
        expression_stack.c
        expression_stack.h
        lexem_string.c
        lexem_string.h
        main.c
        README.md
        scanner.c
        scanner.h
        stack.c
        stack.h
        symtable.c
        symtable.h
        test1.txt
        test2.txt
        code_generator.c
        code_generator.h
        )
