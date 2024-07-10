cmake_minimum_required(VERSION 3.29)
project(ToyCompiler)

set(CMAKE_CXX_STANDARD 20)

add_executable(ToyCompiler main.cpp
        bytecode/chunk.cpp
        bytecode/chunk.h
        common.h)
