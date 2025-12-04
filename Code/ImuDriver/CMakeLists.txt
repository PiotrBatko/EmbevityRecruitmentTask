cmake_minimum_required(VERSION 4.0)

project(Experiments)

find_package(ZeroMQ REQUIRED)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(
    ImuDriver
    main.cpp
)

target_link_libraries(
    ImuDriver
    PRIVATE
        libzmq
        zmqpp
)
