cmake_minimum_required(VERSION 3.15)
project(dumbLoader LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(dumbLoader SHARED
    src/dllmain.cpp
)

set_target_properties(dumbLoader PROPERTIES
    OUTPUT_NAME "XInput1_4"
    PREFIX ""
)

if(WIN32)
    target_compile_definitions(dumbLoader PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
endif()
