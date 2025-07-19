#pragma once
#include <rclcpp/rclcpp.hpp>
#include <optitrack_broadcast/msg/mocap.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <gz/msgs/odometry.pb.h>
#include <iostream>
#include <math_utils.h>
using namespace Eigen;
class Mocap_emulator{
    public:
        Mocap_emulator(const std::string& pub_topic_name,
                       const std::string& sub_topic_name,
                       std::shared_ptr<rclcpp::Node> node,
                       size_t queue_size);
        ~Mocap_emulator();
    private:
        void PublishData();
        void SubscribeFromGazebo(const gz::msgs::Odometry& msg);
        rclcpp::Publisher<optitrack_broadcast::msg::Mocap>::SharedPtr pubmocap_;// publishing the recieved data
        rclcpp::Subscription<gz::msgs::Odometry>::SharedPtr      subgazebo_;// receiving
        optitrack_broadcast::msg::Mocap                             MessageMocap_;
        gz::msgs::Odometry                                    Drone_state_;
        Matrix3f R_IB;
        Vector4f quaternion;
        Vector3f omega_b;// angular velocity in body-fixed frame
        Vector3f omega_i;// angular velocity in inertial frame
};
