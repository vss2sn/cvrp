cmake_minimum_required(VERSION 3.5)
project(cvrp)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

option(VISUALIZE "VISUALIZE" OFF)

if(VISUALIZE)
    # NOTICE requires higher cmake
    message(STATUS "VISUALIZE set to ON. Please run rviz2 in another terminal")
    add_definitions(-DVISUALIZE)
    find_package(ament_cmake REQUIRED)
    find_package(rclcpp REQUIRED)
    find_package(std_msgs REQUIRED)
    find_package(visualization_msgs REQUIRED)
endif(VISUALIZE)

set( CXX_FLAGS "-Wall -Wextra")
set( CMAKE_BUILD_TYPE "Release" )

include_directories( ${CMAKE_SOURCE_DIR}/inc )

file( GLOB LIB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp )
file( GLOB LIB_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/inc/*.hpp )

list( REMOVE_ITEM LIB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp )

add_executable(cvrp ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp ${LIB_SOURCES} ${LIB_HEADERS})

if(VISUALIZE)

  ament_target_dependencies(cvrp rclcpp std_msgs visualization_msgs)

  target_include_directories(cvrp PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

  install(TARGETS cvrp
    EXPORT export_${PROJECT_NAME}
    DESTINATION lib/${PROJECT_NAME})

  if(BUILD_TESTING)
    find_package(ament_lint_auto REQUIRED)
    ament_lint_auto_find_test_dependencies()
  endif()

  ament_package()
endif(VISUALIZE)
