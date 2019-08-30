cmake_minimum_required(VERSION 3.14)
project(sqlite3_ext_md5)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)

set(compile_to_lib 0)

if(${compile_to_lib})
    message(STATUS "compile to dll")
    #set(SOURCE_FILES
    file(GLOB SOURCE_FILES
            md5.c
            )
    add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

else()
    message(STATUS "compile to exe")
    file(GLOB SOURCE_FILES
            main_test_sqlite.cpp
#            main_test_md5.cpp
            sqlite3.c
            md5.c
            )
    add_executable(${PROJECT_NAME} ${SOURCE_FILES})
endif()