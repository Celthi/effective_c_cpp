cmake_minimum_required(VERSION 3.10)

# set the project name
project(main)
# specify the C++ standard
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++2a -stdlib=libc++")

# add the executable

add_executable(co_await_example coroutines/src/co_await_example.cpp)

target_compile_options(co_await_example PRIVATE -fcoroutines-ts)
