cmake_minimum_required(VERSION 3.29)
project(ToyCompiler)

set(CMAKE_CXX_STANDARD 20)

add_executable(ToyCompiler main.cpp
        core/chunk.cpp
        core/chunk.h
        common.h
        core/scanner.cpp
        core/scanner.h
        core/vm.cpp
        core/vm.h
        core/parser.cpp
        core/parser.h
)
