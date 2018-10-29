cmake_minimum_required(VERSION 2.8)

project(BMPRotate)

set(SOURCE_EXE main.c)

set(SOURCE_LIB bmp.c)

add_library(bmp STATIC ${SOURCE_LIB})

add_executable(main ${SOURCE_EXE})

target_link_libraries(main bmp)
