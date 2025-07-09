#pragma once
#include <rclcpp/rclcpp.hpp>
#include <optitrack_broadcast/Mocap.h>
#include <nav_msgs/msg/Odometry.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <iostream>
#include <math_utils.h>
using namespace Eigen;
class Mocap_emulator{
    public:
        Mocap_emulator(const char* PubTopicName,
                       const char* SubTopicName,
                       ros::NodeHandle& n,
                       unsigned int buffersize);
        ~Mocap_emulator();
    private:
        void PublishData();
        void SubscribeFromGazebo(const nav_msgs::Odometry& msg);
        rclcpp::Publisher<optitrack_broadcast::Mocap>SharedPtr pubmocap_;// publishing the recieved data
        rclcpp::Subscription<nav_msgs::Odometry>SharedPtr      subgazebo_;// receiving
        optitrack_broadcast::Mocap                             MessageMocap_;
        nav_msgs::Odometry                                     Drone_state_;
        Matrix3f R_IB;
        Vector4f quaternion;
        Vector3f omega_b;// angular velocity in body-fixed frame
        Vector3f omega_i;// angular velocity in inertial frame
};
