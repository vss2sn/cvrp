cmake_minimum_required(VERSION 3.15)
project(cvrp CXX)

include(cmake/StandardProjectSettings.cmake)
include(cmake/ProjectSpecificSettings.cmake)
include(cmake/PreventInSourceBuilds.cmake)

# Link this 'library' to set the c++ standard / compile-time options requested
add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_17)

if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
  option(ENABLE_BUILD_WITH_TIME_TRACE "Enable -ftime-trace to generate time tracing .json files on clang" OFF)
  if(ENABLE_BUILD_WITH_TIME_TRACE)
    target_compile_options(project_options INTERFACE -ftime-trace)
  endif()
endif()

# Link this 'library' to use the warnings specified in CompilerWarnings.cmake
add_library(project_warnings INTERFACE)

# enable cache system
include(cmake/Cache.cmake)

# standard compiler warnings
include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

# sanitizer options if supported by compiler
include(cmake/Sanitizers.cmake)
enable_sanitizers(project_options)

# allow for static analysis options
include(cmake/StaticAnalyzers.cmake)

file( GLOB LIB_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp )
if(NOT DISPLAY_SOLUTION)
  LIST(REMOVE_ITEM LIB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/graphics_utils.cpp)
endif(NOT DISPLAY_SOLUTION)

add_executable(cvrp main/main.cpp)
target_include_directories(cvrp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_sources(cvrp PRIVATE ${LIB_SOURCES})
target_compile_options (cvrp PRIVATE -O3)
target_link_libraries(cvrp PRIVATE project_options project_warnings)

if(DISPLAY_SOLUTION)
  target_link_libraries(cvrp PRIVATE sfml-graphics sfml-window sfml-system)
endif (DISPLAY_SOLUTION)
