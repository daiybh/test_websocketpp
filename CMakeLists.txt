cmake_minimum_required(VERSION 3.15)
project(hello CXX)
set(CMAKE_CXX_STANDARD 20)
add_definitions(-DASIO_STANDALONE)
add_definitions(-D_WEBSOCKETPP_CPP11_TYPE_TRAITS_)

add_executable(broadcast_server "src/broadcast_server.cpp")
target_include_directories(broadcast_server PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")


add_executable(simple_broadcast_server "src/simple_broadcast_server.cpp")
target_include_directories(simple_broadcast_server PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")


