cmake_minimum_required(VERSION 3.14)
project(sqlite3_ext_md5)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)

#set(SOURCE_FILES
file(GLOB SOURCE_FILES
        main_test_sqlite.cpp
#        main_test_md5.cpp
#        sqlite3.h
        sqlite3.c
#        md5.h
        md5.c
        #        sqlite3ext.h
)

add_executable(sqlite3_ext_md5 ${SOURCE_FILES})

#add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
