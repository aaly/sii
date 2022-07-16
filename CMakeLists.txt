cmake_minimum_required(VERSION 3.10)

# set the project name
project(sii)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


# add the executable
add_executable(sii src/main.cpp src/service_manager.cpp src/common.cpp)
target_include_directories(sii PUBLIC "include")
#target_link_libraries(sii dr_echo)
