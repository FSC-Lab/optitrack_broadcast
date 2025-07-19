#include "Mocap_emulator.hpp"

Mocap_emulator::Mocap_emulator(const std::string& pub_topic_name,
                               const std::string& sub_topic_name,
                               std::shared_ptr<rclcpp::Node> node,
                               size_t queue_size)
{
    pubmocap_ = node->create_publisher<optitrack_broadcast::msg::Mocap>(pub_topic_name, queue_size);

    subgazebo_ = node->create_subscription<gz::msgs::Odometry>(
        sub_topic_name,
        queue_size,
        std::bind(&Mocap_emulator::SubscribeFromGazebo, this, std::placeholders::_1));
}

Mocap_emulator::~Mocap_emulator()
{

}
void Mocap_emulator::PublishData()
{
    MessageMocap_.pose.position.x = Drone_state_.pose().position().x();
    MessageMocap_.pose.position.y = Drone_state_.pose().position().y();
    MessageMocap_.pose.position.z = Drone_state_.pose().position().z();
    MessageMocap_.twist.linear.x = Drone_state_.twist().linear().x();
    MessageMocap_.twist.linear.y = Drone_state_.twist().linear().y();
    MessageMocap_.twist.linear.z = Drone_state_.twist().linear().z();

    MessageMocap_.pose.orientation.w = Drone_state_.pose().orientation().w();
    MessageMocap_.pose.orientation.x = Drone_state_.pose().orientation().x();
    MessageMocap_.pose.orientation.y = Drone_state_.pose().orientation().y();
    MessageMocap_.pose.orientation.z = Drone_state_.pose().orientation().z();
    
    quaternion(0) = MessageMocap_.pose.orientation.w;
    quaternion(1) = MessageMocap_.pose.orientation.x;
    quaternion(2) = MessageMocap_.pose.orientation.y;
    quaternion(3) = MessageMocap_.pose.orientation.z;

    R_IB = QuaterionToRotationMatrix(quaternion);
    // the angular velocity from gazebo is in inertial frame
    omega_i(0) = Drone_state_.twist().angular().x();
    omega_i(1) = Drone_state_.twist().angular().y();
    omega_i(2) = Drone_state_.twist().angular().z();

    omega_b = R_IB.transpose() * omega_i; 
    // publish angular velocity in body-fixed frame
    MessageMocap_.twist.angular.x = omega_b(0);
    MessageMocap_.twist.angular.y = omega_b(1);
    MessageMocap_.twist.angular.z = omega_b(2);

    const gz::msgs::Header& header = Drone_state_.header();
    MessageMocap_.header.stamp.sec = header.stamp().sec();
    MessageMocap_.header.stamp.nanosec = header.stamp().nsec();
    for (int i = 0; i < header.data_size(); ++i)
    {
        const gz::msgs::Header::Map& field = header.data(i);
        if (field.key() == "frame_id")
        {
            std::string frame_id = field.value(i);
            MessageMocap_.header.frame_id = frame_id;
        }
    }
    pubmocap_->publish(MessageMocap_);
}
void Mocap_emulator::SubscribeFromGazebo(const gz::msgs::Odometry& msg)
{
    // waiting for plugin message
    Drone_state_ = msg;
    // publish the plugin message
    PublishData();
}
