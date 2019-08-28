cmake_minimum_required(VERSION 3.14)
project(md5 C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(md5
        md5.c
        md5.h
        sqlite3.h
        sqlite3ext.h)
