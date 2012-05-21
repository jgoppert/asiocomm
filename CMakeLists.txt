cmake_minimum_required(VERSION 2.8)

project(asiocomm)

find_package(Boost 1.42 REQUIRED COMPONENTS thread system)

include_directories(${Boost_INCLUDE_DIRS})
link_libraries(${Boost_LIBRARIES})

#add_executable(test
    #AsyncSerial.hpp
    #AsyncSerial.cpp
    #AsyncUDP.hpp
    #AsyncUDP.cpp
    #)
add_executable(tcp_echo_server tcp_echo_server.cpp)
add_executable(udp_echo_server udp_echo_server.cpp)
