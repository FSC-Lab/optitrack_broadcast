cmake_minimum_required(VERSION 3.15)
project(optitrack_broadcast)

# Default to C++17
set(CMAKE_CXX_STANDARD 17)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

option(OB_WITH_GAZEBO "Enable gazebo support for mocap emulation" ON)

if(OB_WITH_GAZEBO)
  find_package(gz-msgs10 REQUIRED)
  message(STATUS "cmake prefix: " ${CMAKE_PREFIX_PATH})
endif()

find_package(ament_cmake REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(fsc_autopilot_ros2_msgs REQUIRED)
find_package(Eigen3 REQUIRED)

# System dependencies are found with CMake's conventions
find_package(Boost REQUIRED COMPONENTS system)

set(_ob_deps
    ament_cmake
    rclcpp
    std_msgs
    geometry_msgs
    tf2_ros
    tf2_eigen
    nav_msgs
    rosidl_default_generators)

if(OB_WITH_GAZEBO)
  list(APPEND _ob_deps gz-msgs10)
endif()

# export
ament_export_dependencies(${_ob_deps})


# add optitrackFeedBackRigidBody.cpp
add_library(OptiTrackFeedBackRigidBody src/lib/OptiTrackFeedBackRigidBody.cpp)
target_link_libraries(OptiTrackFeedBackRigidBody Eigen3::Eigen)
target_include_directories(OptiTrackFeedBackRigidBody PUBLIC include/)
ament_target_dependencies(OptiTrackFeedBackRigidBody rclcpp geometry_msgs
                          fsc_autopilot_ros2_msgs)

# add optitrackPublisher.cpp
add_library(OptiTrackPublisher src/lib/OptiTrackPublisher.cpp)
target_link_libraries(OptiTrackPublisher Eigen3::Eigen)
target_include_directories(OptiTrackPublisher PUBLIC include/)
ament_target_dependencies(OptiTrackPublisher rclcpp geometry_msgs
                          fsc_autopilot_ros2_msgs)

# add broadcast_position_velocity.cpp
add_executable(broadcast_position_velocity src/broadcast_position_velocity.cpp)
target_link_libraries(broadcast_position_velocity OptiTrackFeedBackRigidBody
                      OptiTrackPublisher)
ament_target_dependencies(broadcast_position_velocity rclcpp geometry_msgs
                          fsc_autopilot_ros2_msgs)

set(_ob_list
  OptiTrackFeedBackRigidBody
  OptiTrackPublisher
  broadcast_position_velocity
)
install(TARGETS ${_ob_list} DESTINATION lib/${PROJECT_NAME})

install(FILES package.xml DESTINATION share/${PROJECT_NAME})

ament_package()
