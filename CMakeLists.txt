project(vrp)

cmake_minimum_required(VERSION 3.5)
add_definitions(-std=c++11)

set( CXX_FLAGS "-w")
set( CMAKE_CXX_FLAGS "${CXX_FLAGS} -std=c++11")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS 1)

include_directories( ${CMAKE_SOURCE_DIR}/inc )

file( GLOB LIB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp )
file( GLOB LIB_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/inc/*.hpp )

# Building utils.cpp as  a library as common to multiple files
add_library( utils SHARED ${CMAKE_CURRENT_SOURCE_DIR}/src/utils.cpp ${LIB_HEADERS} )
list( REMOVE_ITEM LIB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/utils.cpp )

foreach(test_source_file ${LIB_SOURCES})
  string( REPLACE ".cpp" "" test_output_file ${test_source_file} )
  string( REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/src/" "" test_output_file ${test_output_file} )
  add_executable( ${test_output_file} ${test_source_file} ${LIB_HEADERS})
  target_link_libraries( ${test_output_file} utils )
endforeach(test_source_file ${LIB_SOURCES})
