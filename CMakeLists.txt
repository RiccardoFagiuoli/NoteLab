cmake_minimum_required(VERSION 4.0)
project(NoteLab)

set(CMAKE_CXX_STANDARD 20)

add_executable(NoteLab main.cpp
        Note.h
        Note.cpp
        Observer.h
        Collezioni.h
        Collezioni.cpp)
