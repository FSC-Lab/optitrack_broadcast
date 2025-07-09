#pragma once
#include <rclcpp/rclcpp.hpp>
#include <optitrack_broadcast/msg/Mocap.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <OptiTrackFeedBackRigidBody.h>
#include <iostream>
using namespace Eigen;
class OptiTrackPublisher{
    public: 
        OptiTrackPublisher(const char* TopicName,
                           std::shared_ptr<rclcpp::Node> node, 
                           unsigned int buffersize, 
                           const char* MessageType);
        ~OptiTrackPublisher();
        void PublishData(rigidbody_state& StateInput);
    private:
        rclcpp::Publisher publisher_; 
        int messagetype_;
        optitrack_broadcast::Mocap MessageMocap_;
        nav_msgs::msg::Odometry  MessageOdometry_;   
        geometry_msgs::msg::Twist  MessageTwist_;
        double position[3];
        double velocity[3];
        double angular_velocity[3];
        double quaternion[4];
};


