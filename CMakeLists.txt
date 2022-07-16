cmake_minimum_required(VERSION 3.11)

# set the project name
project(sii)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_BUILD_TYPE RELEASE)

include(FetchContent)

FetchContent_Declare(zenohpico
    GIT_REPOSITORY https://github.com/eclipse-zenoh/zenoh-pico.git
    GIT_TAG 0.5.0-beta.9)

#FetchContent_Populate(zenohpico)
#FetchContent_MakeAvailable(zenohpico)

if(NOT zenohpico_POPULATED)
    message(STATUS "populating zenohpico")
    FetchContent_Populate(zenohpico)
    add_subdirectory(${zenohpico_SOURCE_DIR} ${zenohpico_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

file(GLOB ZENOHPICO_SOURCES "${zenohpico_SOURCE_DIR}/src/*.c"
                     "${zenohpico_SOURCE_DIR}/src/api/*.c"
                     "${zenohpico_SOURCE_DIR}/src/collections/*.c"
                     "${zenohpico_SOURCE_DIR}/src/link/*.c"
                     "${zenohpico_SOURCE_DIR}/src/link/config/*.c"
                     "${zenohpico_SOURCE_DIR}/src/link/unicast/*.c"
                     "${zenohpico_SOURCE_DIR}/src/link/multicast/*.c"
                     "${zenohpico_SOURCE_DIR}/src/protocol/*.c"
                     "${zenohpico_SOURCE_DIR}/src/zenoh/protocol/*.c"
                     "${zenohpico_SOURCE_DIR}/src/session/*.c"
                     "${zenohpico_SOURCE_DIR}/src/system/*.c"
                     "${zenohpico_SOURCE_DIR}/src/system/unix/*.c"
                     "${zenohpico_SOURCE_DIR}/src/transport/*.c"
                     "${zenohpico_SOURCE_DIR}/src/transport/common/*.c"
                     "${zenohpico_SOURCE_DIR}/src/transport/unicast/*.c"
                     "${zenohpico_SOURCE_DIR}/src/transport/unicast/link/*.c"
                     "${zenohpico_SOURCE_DIR}/src/transport/unicast/link/task/*.c"
                     "${zenohpico_SOURCE_DIR}/src/transport/multicast/*.c"
                     "${zenohpico_SOURCE_DIR}/src/transport/multicast/link/*.c"
                     "${zenohpico_SOURCE_DIR}/src/transport/multicast/link/task/*.c")

# add the executable
add_executable(sii src/main.cpp src/service_manager.cpp src/common.cpp ${ZENOHPICO_SOURCES})
target_include_directories(sii PUBLIC "include" ${zenohpico_SOURCE_DIR}/include)
target_compile_definitions(sii PUBLIC -DZENOH_LINUX)
#target_link_libraries(sii zenohpico)
