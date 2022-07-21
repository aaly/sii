cmake_minimum_required(VERSION 3.11)

# set the project name
project(eval)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_BUILD_TYPE RELEASE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-pie")


# add the executable
add_executable(eval src/eval.cpp src/eval.cpp src/ src/duktape.c)
target_include_directories(eval PUBLIC "include")
