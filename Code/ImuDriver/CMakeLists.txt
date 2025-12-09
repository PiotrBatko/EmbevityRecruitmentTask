cmake_minimum_required(VERSION 4.0)

project(Experiments)

find_package(ZeroMQ REQUIRED)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(ImuDriver)

target_link_libraries(
    ImuDriver
    PRIVATE
        libzmq
        zmqpp
)

target_sources(
    ImuDriver
    PRIVATE
        main.cpp

        # Common
        Logger.cpp
        Register.cpp

        # Implementation
        FreeFallDetector.cpp
        FreeFallLogger.cpp
        ImuDriver.cpp
        SimulatedI2c.cpp

        # View
        UserInterface.cpp
)

target_include_directories(
    ImuDriver
    PUBLIC
        Include
)
