cmake_minimum_required(VERSION 3.5)
project(cvrp)

file( GLOB LIB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp )
add_executable(cvrp main/main.cpp)
target_include_directories(cvrp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_sources(cvrp PRIVATE ${LIB_SOURCES})
set_target_properties(cvrp
  PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    CXX_FLAGS "${CMAKE_CXX_FLAGS}"
)
target_compile_options (cvrp PRIVATE -O3)
