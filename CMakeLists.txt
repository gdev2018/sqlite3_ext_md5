cmake_minimum_required(VERSION 3.14)
project(sqlite3_ext_md5)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)

file(GLOB SOURCE_FILES
        test.cpp
        md5.c
        md5.h
        md5tcl.c
        sqlite3.h
        sqlite3ext.h)

add_executable(sqlite3_ext_md5 ${SOURCE_FILES})

#add_library(sqlite3_ext_md5 SHARED ${SOURCE_FILES})