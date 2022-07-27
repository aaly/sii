cmake_minimum_required(VERSION 3.11)

# set the project name
project(sii)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_BUILD_TYPE DEBUG)

include(FetchContent)

FetchContent_Declare(zenohpico
    GIT_REPOSITORY https://github.com/eclipse-zenoh/zenoh-pico.git
    GIT_TAG 0.5.0-beta.9)

if(NOT zenohpico_POPULATED)
    message(STATUS "populating zenohpico")
    FetchContent_Populate(zenohpico)
    add_subdirectory(${zenohpico_SOURCE_DIR} ${zenohpico_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

file(GLOB ZENOHPICO_SOURCES "${zenohpico_SOURCE_DIR}/src/*.c"
                     "${zenohpico_SOURCE_DIR}/src/compat/*.c"
                     "${zenohpico_SOURCE_DIR}/src/net/*.c"
                     "${zenohpico_SOURCE_DIR}/src/net/unix/*.c"
                     "${zenohpico_SOURCE_DIR}/src/api/*.c"
                     "${zenohpico_SOURCE_DIR}/src/collections/*.c"
                     "${zenohpico_SOURCE_DIR}/src/zenoh/link/*.c"
                     "${zenohpico_SOURCE_DIR}/src/zenoh/link/config/*.c"
                     "${zenohpico_SOURCE_DIR}/src/zenoh/link/unicast/*.c"
                     "${zenohpico_SOURCE_DIR}/src/zenoh/link/multicast/*.c"
                     "${zenohpico_SOURCE_DIR}/src/protocol/*.c"
                     "${zenohpico_SOURCE_DIR}/src/zenoh/protocol/*.c"
                     "${zenohpico_SOURCE_DIR}/src/zenoh/link/*.c"
                     "${zenohpico_SOURCE_DIR}/src/zenoh/*.c"
                     "${zenohpico_SOURCE_DIR}/src/zenoh/session/*.c"
                     "${zenohpico_SOURCE_DIR}/src/system/*.c"
                     "${zenohpico_SOURCE_DIR}/src/system/unix/*.c"
                     "${zenohpico_SOURCE_DIR}/src/zenoh/transport/*.c"
                     "${zenohpico_SOURCE_DIR}/src/zenoh/transport/link/*.c"
                     "${zenohpico_SOURCE_DIR}/src/zenoh/transport/link/task/*.c"
                     "${zenohpico_SOURCE_DIR}/src/zenoh/transport/common/*.c"
                     "${zenohpico_SOURCE_DIR}/src/zenoh/transport/unicast/*.c"
                     "${zenohpico_SOURCE_DIR}/src/zenoh/transport/unicast/link/*.c"
                     "${zenohpico_SOURCE_DIR}/src/zenoh/transport/unicast/link/task/*.c"
                     "${zenohpico_SOURCE_DIR}/src/zenoh/transport/multicast/*.c"
                     "${zenohpico_SOURCE_DIR}/src/zenoh/transport/multicast/link/*.c"
                     "${zenohpico_SOURCE_DIR}/src/zenoh/transport/multicast/link/task/*.c")
message(WARNING "zenohpico files: ${ZENOHPICO_SOURCES}")

add_library(zenohpico_lib STATIC ${ZENOHPICO_SOURCES})
target_include_directories(zenohpico_lib PUBLIC "include" ${zenohpico_SOURCE_DIR}/include)
target_compile_definitions(zenohpico_lib PUBLIC -DZENOH_LINUX)

# add the executable
add_executable(sii src/main.cpp src/service_manager.cpp src/service.cpp src/common.cpp)
target_include_directories(sii PUBLIC "include" ${zenohpico_SOURCE_DIR}/include)
target_compile_definitions(sii PUBLIC -DZENOH_LINUX stdc++fs)
target_link_libraries(sii pthread zenohpico_lib)
