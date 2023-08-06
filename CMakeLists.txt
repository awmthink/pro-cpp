cmake_minimum_required(VERSION 3.20)
project(procpp)
enable_language(CXX)

set(CMAKE_C_COMPILER /data/envs/gcc-13.1.0/bin/gcc)
set(CMAKE_CXX_COMPILER /data/envs/gcc-13.1.0/bin/g++)
link_directories(/data/envs/gcc-13.1.0/lib64)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(sources)