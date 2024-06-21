cmake_minimum_required(VERSION 3.15)
project(hello CXX)
add_definitions(-DASIO_STANDALONE)
add_definitions(-D_WEBSOCKETPP_CPP11_TYPE_TRAITS_)

add_executable(echo_server src/echo_server.cpp)
target_include_directories(echo_server PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")


