cmake_minimum_required(VERSION 4.0)
project(NoteLab)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)


find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

add_subdirectory(googletest)

include_directories(googletest/googletest/include googletest/googlemock/include)

add_library(NoteCore
Note.cpp
        Note.h
        Collezioni.h
Collezioni.cpp)

add_executable(NoteLab main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui)
target_link_libraries(NoteLab PRIVATE
        NoteCore
Qt6::Widgets)

add_executable(run_tests tests.cpp)
target_link_libraries(run_tests PRIVATE NoteCore gtest gtest_main)

target_include_directories(NoteCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(NoteLab PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(run_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

qt_finalize_executable(NoteLab)