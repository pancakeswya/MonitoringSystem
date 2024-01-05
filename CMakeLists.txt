cmake_minimum_required(VERSION 3.28)
project(untitled25 LANGUAGES C)

set(CMAKE_C_STANDARD 11)

include_directories(".")

add_executable(untitled25 main.c
        base/util.c
        base/util.h
        agents/cpu.c
        agents/cpu.h
        agents/memory.c
        agents/memory.h
)
