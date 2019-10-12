project(cvrp)

cmake_minimum_required(VERSION 3.5)
add_definitions(-std=c++11)

set( CXX_FLAGS "-w")
set( CMAKE_CXX_FLAGS "${CXX_FLAGS} -std=c++11 -g")

# set(CMAKE_CXX_STANDARD 11)
# set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS 1)

include_directories( ${CMAKE_SOURCE_DIR}/inc )

file( GLOB LIB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp )
file( GLOB LIB_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/inc/*.hpp )

list( REMOVE_ITEM LIB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp )

add_executable(cvrp ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp ${LIB_SOURCES} ${LIB_HEADERS})
