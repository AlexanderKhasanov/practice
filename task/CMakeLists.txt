cmake_minimum_required(VERSION 3.14)
project(task)

find_package(Boost 1.57.0 COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 14)

add_executable(task main.cpp auto_logs.hpp auto_logs.cpp)

target_link_libraries( task ${Boost_LIBRARIES} )