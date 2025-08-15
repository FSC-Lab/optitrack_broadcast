#pragma once
#include <rclcpp/rclcpp.hpp>
#include "fsc_autopilot_ros2_msgs/msg/mocap.hpp"
#include <nav_msgs/msg/odometry.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include "OptiTrackFeedBackRigidBody.hpp"
#include <iostream>
using namespace Eigen;
class OptiTrackPublisher{
    public: 
        OptiTrackPublisher(const char* TopicName,
                           rclcpp::Node& node, 
                           unsigned int buffersize);
        ~OptiTrackPublisher();
        void PublishData(rigidbody_state& StateInput);
    private:
        
        rclcpp::Publisher<fsc_autopilot_ros2_msgs::msg::Mocap>::SharedPtr publisher_;
        int messagetype_;
        fsc_autopilot_ros2_msgs::msg::Mocap MessageMocap_;
        nav_msgs::msg::Odometry  MessageOdometry_;   
        geometry_msgs::msg::Twist  MessageTwist_;
        double position[3];
        double velocity[3];
        double angular_velocity[3];
        double quaternion[4];
};


