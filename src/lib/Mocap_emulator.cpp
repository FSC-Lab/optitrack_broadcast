#include "Mocap_emulator.h"

Mocap_emulator::Mocap_emulator(const std::string& pub_topic_name,
                               const std::string& sub_topic_name,
                               std::shared_ptr<rclcpp::Node> node,
                               size_t queue_size)
{
    pubmocap_ = node->create_publisher<optitrack_broadcast::Mocap>(pub_topic_name, queue_size);

    subgazebo_ = node->create_subscription<geometry_msgs::msg::PoseStamped>(
        sub_topic_name,
        queue_size,
        std::bind(&Mocap_emulator::SubscribeFromGazebo, this, std::placeholders::_1));
}

Mocap_emulator::~Mocap_emulator()
{

}
void Mocap_emulator::PublishData()
{
    MessageMocap_.position[0] = Drone_state_.pose.pose.position.x;
    MessageMocap_.position[1] = Drone_state_.pose.pose.position.y;
    MessageMocap_.position[2] = Drone_state_.pose.pose.position.z;
    MessageMocap_.velocity[0] = Drone_state_.twist.twist.linear.x;
    MessageMocap_.velocity[1] = Drone_state_.twist.twist.linear.y;
    MessageMocap_.velocity[2] = Drone_state_.twist.twist.linear.z;

    MessageMocap_.quaternion[0] = Drone_state_.pose.pose.orientation.w;
    MessageMocap_.quaternion[1] = Drone_state_.pose.pose.orientation.x;
    MessageMocap_.quaternion[2] = Drone_state_.pose.pose.orientation.y;
    MessageMocap_.quaternion[3] = Drone_state_.pose.pose.orientation.z;

    quaternion(0) = MessageMocap_.quaternion[0];
    quaternion(1) = MessageMocap_.quaternion[1];
    quaternion(2) = MessageMocap_.quaternion[2];
    quaternion(3) = MessageMocap_.quaternion[3];

    R_IB = QuaterionToRotationMatrix(quaternion);
    // the angular velocity from gazebo is in inertial frame
    omega_i(0) = Drone_state_.twist.twist.angular.x;
    omega_i(1) = Drone_state_.twist.twist.angular.y;
    omega_i(2) = Drone_state_.twist.twist.angular.z;

    omega_b = R_IB.transpose() * omega_i; 
    // publish angular velocity in body-fixed frame
    MessageMocap_.angular_velocity[0] = omega_b(0);
    MessageMocap_.angular_velocity[1] = omega_b(1);
    MessageMocap_.angular_velocity[2] = omega_b(2);

    MessageMocap_.header = Drone_state_.header;
    pubmocap_.publish(MessageMocap_);
}
void Mocap_emulator::SubscribeFromGazebo(const nav_msgs::msg::Odometry& msg)
{
    // waiting for plugin message
    Drone_state_ = msg;
    // publish the plugin message
    PublishData();
}
