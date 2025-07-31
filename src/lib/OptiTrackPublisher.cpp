#include "OptiTrackPublisher.hpp"

OptiTrackPublisher::OptiTrackPublisher(const char* TopicName,
                                       rclcpp::Node& node, 
                                       unsigned int buffersize)
{
    publisher_ = node.create_publisher<optitrack_broadcast::msg::Mocap>(TopicName, buffersize);
}
OptiTrackPublisher::~OptiTrackPublisher()
{

}
void OptiTrackPublisher::PublishData(rigidbody_state& StateInput)
{
    for ( int i = 0; i < 3; i++) {
        position[i] = StateInput.Position(i);
        velocity[i] = StateInput.V_I(i);
        angular_velocity[i] = StateInput.Omega_BI(i);
    }
    for( int i = 0; i < 4; i++) {
        quaternion[i] = StateInput.quaternion(i);
    }    
    
    MessageMocap_.pose.position.x = StateInput.Position(0);
    MessageMocap_.pose.position.y = StateInput.Position(1);
    MessageMocap_.pose.position.z = StateInput.Position(2);
    MessageMocap_.twist.linear.x = StateInput.V_I(0);
    MessageMocap_.twist.linear.y = StateInput.V_I(1);
    MessageMocap_.twist.linear.z = StateInput.V_I(2);
    MessageMocap_.twist.angular.x = StateInput.Omega_BI(0);
    MessageMocap_.twist.angular.y = StateInput.Omega_BI(1);
    MessageMocap_.twist.angular.z = StateInput.Omega_BI(2);
    MessageMocap_.pose.orientation.w = StateInput.quaternion(0);
    MessageMocap_.pose.orientation.x = StateInput.quaternion(1);
    MessageMocap_.pose.orientation.y = StateInput.quaternion(2);
    MessageMocap_.pose.orientation.z = StateInput.quaternion(3);
    
    rclcpp::Clock steady_clock(RCL_ROS_TIME);  

    MessageMocap_.header.stamp = steady_clock.now();
    publisher_->publish(MessageMocap_);
}
