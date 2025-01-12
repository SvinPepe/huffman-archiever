cmake_minimum_required(VERSION 3.30)
project(huffman)

set(CMAKE_CXX_STANDARD 23)

add_executable(huffman main.cpp
        Archiever.cpp
        Archiever.h
        Adaptive_Archiever.cpp
        Adaptive_Archiever.h
)
