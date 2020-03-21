cmake_minimum_required(VERSION 3.16)
project(441asg3RoutingTradeoffs)

set(CMAKE_CXX_STANDARD 14)

add_executable(441asg3RoutingTradeoffs main.cpp Result.cpp Result.h Event.cpp Event.h Edge.cpp Edge.h)
#add_executable(441asg3RoutingTradeoffs Result.cpp Result.h Event.cpp Event.h  nonclass.cpp )