cmake_minimum_required(VERSION 4.0)
project(NoteLab)

set(CMAKE_CXX_STANDARD 20)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

add_subdirectory(googletest)

include_directories(googletest/googletest/include googletest/googlemock/include)

add_library(NoteCore
        Note.cpp
        Collezioni.cpp)

add_executable(NoteLab main.cpp
        NoteCounter.h
)
target_link_libraries(NoteLab NoteCore)

add_executable(run_tests tests.cpp)
target_link_libraries(run_tests NoteCore gtest gtest_main)