cmake_minimum_required(VERSION 3.5)
project(cvrp)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

option(VISUALIZE "VISUALIZE" OFF)

set( CXX_FLAGS "-Wall -Wextra")
set( CMAKE_BUILD_TYPE "Release" )

include_directories( ${CMAKE_SOURCE_DIR}/inc )

file( GLOB LIB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp )
file( GLOB LIB_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/inc/*.hpp )

list( REMOVE_ITEM LIB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp )

add_executable(cvrp ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp ${LIB_SOURCES} ${LIB_HEADERS})
