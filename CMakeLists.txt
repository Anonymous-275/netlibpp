cmake_minimum_required(VERSION 3.16)
project(netlibpp)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

INCLUDE(CheckIncludeFiles)
CHECK_INCLUDE_FILES(winsock2.h WINSOCK)

add_library(netlibpp STATIC netlibpp.h netlibpp_enums.h netlibpp.cpp)

if(WINSOCK)
    target_link_libraries(netlibpp ws2_32)
    if(UNIX) #most likely MINGW or other GNU-Windows capable compiler running on UNIX
        add_definitions("-D_WIN32_WINNT=0x0600")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s --static")
    endif(UNIX)
endif(WINSOCK)

add_executable(netlibpp_tcp_client_example examples/tcp_client_example.cpp)
target_link_libraries(netlibpp_tcp_client_example netlibpp)

add_executable(netlibpp_udp_client_example examples/udp_client_example.cpp)
target_link_libraries(netlibpp_udp_client_example netlibpp)

add_executable(netlibpp_enforce_length_example examples/enforce_length_example.cpp)
target_link_libraries(netlibpp_enforce_length_example netlibpp)