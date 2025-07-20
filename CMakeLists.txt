cmake_minimum_required(VERSION 3.8)
project(optitrack_broadcast)

# Default to C++17
set(CMAKE_CXX_STANDARD 17)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CMAKE_PREFIX_PATH "/usr/lib/x86_64-linux-gnu/cmake/gz-msgs10")
message(STATUS "cmake prefix: " ${CMAKE_PREFIX_PATH})

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_eigen REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(eigen3_cmake_module REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(gz-msgs10 REQUIRED)
## System dependencies are found with CMake's conventions
find_package(Boost REQUIRED COMPONENTS system)

rosidl_generate_interfaces(${PROJECT_NAME}
   "msg/Mocap.msg"
   DEPENDENCIES geometry_msgs nav_msgs std_msgs
)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)

# add message
rosidl_get_typesupport_target(
  cpp_typesupport_target
  ${PROJECT_NAME} 
  rosidl_typesupport_cpp
)

# add mocap_emulator.cpp
add_library(Mocap_emulator src/lib/Mocap_emulator.cpp)
target_link_libraries(Mocap_emulator 
  ${cpp_typesupport_target} 
  Eigen3::Eigen
)  
ament_target_dependencies(Mocap_emulator rclcpp geometry_msgs nav_msgs gz-msgs10)
install(TARGETS Mocap_emulator DESTINATION lib/${PROJECT_NAME})

# add optitrackFeedBackRigidBody.cpp
add_library(OptiTrackFeedBackRigidBody src/lib/OptiTrackFeedBackRigidBody.cpp)
target_link_libraries(OptiTrackFeedBackRigidBody 
  ${cpp_typesupport_target} 
  Eigen3::Eigen
)
target_include_directories(OptiTrackFeedBackRigidBody PUBLIC include/)
ament_target_dependencies(OptiTrackFeedBackRigidBody rclcpp geometry_msgs)
install(TARGETS OptiTrackFeedBackRigidBody DESTINATION lib/${PROJECT_NAME})

# add optitrackPublisher.cpp
add_library(OptiTrackPublisher src/lib/OptiTrackPublisher.cpp)
target_link_libraries(OptiTrackPublisher 
  ${cpp_typesupport_target} 
  Eigen3::Eigen
)
target_include_directories(OptiTrackPublisher PUBLIC include/)
ament_target_dependencies(OptiTrackPublisher rclcpp geometry_msgs)
install(TARGETS OptiTrackPublisher DESTINATION lib/${PROJECT_NAME})

# add broadcast_position_velocity.cpp
add_executable(broadcast_position_velocity src/broadcast_position_velocity.cpp)
target_link_libraries(broadcast_position_velocity
  ${rclcpp_LIBRARIES}
  ${rosidl_default_runtime_LIBRARIES}
  OptiTrackFeedBackRigidBody
  OptiTrackPublisher
)
target_link_libraries(broadcast_position_velocity 
  ${cpp_typesupport_target}
)
ament_target_dependencies(broadcast_position_velocity rclcpp geometry_msgs)
install(TARGETS broadcast_position_velocity DESTINATION lib/${PROJECT_NAME})

# add emulator_position_velocity.cpp
add_executable(emulator_position_velocity src/emulator_position_velocity.cpp)
target_link_libraries(emulator_position_velocity
  ${rclcpp_LIBRARIES}
  ${rosidl_default_runtime_LIBRARIES}
  Mocap_emulator
)
target_link_libraries(emulator_position_velocity
  ${cpp_typesupport_target}
)
ament_target_dependencies(emulator_position_velocity rclcpp geometry_msgs)
install(TARGETS emulator_position_velocity DESTINATION lib/${PROJECT_NAME})

# export
ament_export_dependencies(ament_cmake rclcpp std_msgs rosidl_default_runtime eigen3_cmake_module Eigen3 px4_msgs geometry_msgs sensor_msgs tf2_ros tf2_eigen nav_msgs gz-msgs10 eigen3_cmake_module Eigen3)

ament_package()
