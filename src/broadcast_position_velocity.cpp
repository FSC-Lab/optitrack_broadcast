#include <rclcpp/rclcpp.hpp>
#include <iostream>
#include <Eigen/Eigen>
#include <memory>
#include <vector>
#include <string>
#include <cstring>
#include "OptiTrackFeedBackRigidBody.hpp"
#include "OptiTrackPublisher.hpp"

using namespace std::chrono_literals;

struct SubTopic {
    char str[100];
};

struct PubTopic {
    char str[100];
};

class OptiTrackBroadcastNode : public rclcpp::Node {
public:
    OptiTrackBroadcastNode(int argc, char** argv) : Node("broadcast_position_velocity") {
        // Frequency parameter
        double frequency = this->declare_parameter("frequency", 60.0);
        rate_ = std::make_shared<rclcpp::Rate>(frequency);

        // Message type parameter
        const std::string default_msg_type = "Mocap";
        msg_type_ = this->declare_parameter("msg_type", default_msg_type);
        
        notfeedbackcounter_ = 2;
        num_channels_ = argc - 1;

        // Must have at least 1 input argument for topic name
        if (argc > 1) {
            RCLCPP_INFO(this->get_logger(), "++++++++RECEIVEING MOCAP INFO FROM THE FOLLOWING TOPICS:++++++++");
            
            for (int i = 1; i < argc; i++) {
                // Create subscriber topic
                SubTopic topic;
                strcpy(topic.str, "/vrpn_client_node/");
                strcat(topic.str, argv[i]);
                strcat(topic.str, "/pose");
                topic_list_.push_back(topic);
                
                // Create OptiTrack feedback instance
                mocap_list_.emplace_back(
                    std::make_unique<OptiTrackFeedBackRigidBody>(
                        topic.str, this->shared_from_this(), 3, 3));
                
                // Initialize state
                rigidbody_state init_state;
                mocap_list_.back()->GetState(init_state);
                rigidbody_state_list_.push_back(init_state);

                // Initialize mocap message
                optitrack_broadcast::msg::Mocap init_mocap;
                init_mocap.pose.position.x = 0;
                init_mocap.pose.position.y = 0;
                init_mocap.pose.position.z = 0;
                init_mocap.twist.linear.x = 0;
                init_mocap.twist.linear.y = 0;
                init_mocap.twist.linear.z = 0;
                init_mocap.twist.angular.x = 0;
                init_mocap.twist.angular.y = 0;
                init_mocap.twist.angular.z = 0;
                init_mocap.pose.orientation.w = init_state.quaternion(0);
                init_mocap.pose.orientation.x = init_state.quaternion(1);
                init_mocap.pose.orientation.y = init_state.quaternion(2);
                init_mocap.pose.orientation.z = init_state.quaternion(3);
                mocap_message_list_.push_back(init_mocap);

                // Create publisher topic
                PubTopic pub_topic;
                strcpy(pub_topic.str, "/mocap/");
                strcat(pub_topic.str, argv[i]);
                pub_list_.emplace_back(
                    std::make_unique<OptiTrackPublisher>(
                        pub_topic.str, this->shared_from_this(), 1000));
                
                RCLCPP_INFO(this->get_logger(), "%s publish processed data to: %s", 
                            topic.str, pub_topic.str);
            }
        } else {
            RCLCPP_WARN(this->get_logger(), "NO TOPIC NAME SPECIFIED!");
        }

        // Create timer for main loop
        timer_ = this->create_wall_timer(
            1s / frequency,
            std::bind(&OptiTrackBroadcastNode::main_loop, this));
    }

private:
    void main_loop() {
        bool is_feedback_state_changed = false;
        bool is_feedback_all_good = true;

        if (num_channels_ > 0) {
            for (int k = 0; k < num_channels_; k++) {
                // Update the state for each optitrack instance
                mocap_list_[k]->FeedbackDetector(notfeedbackcounter_); // store the previous state
                bool feedback_state_temp = rigidbody_state_list_[k].isFeedbackNomral;
                mocap_list_[k]->GetState(rigidbody_state_list_[k]);
                
                // If the buffer number is equal to or above the threshold, set the flag to false
                if (!rigidbody_state_list_[k].isFeedbackNomral) {
                    is_feedback_all_good = false;
                }
                
                // Detect any changes to the optitrack feedback
                if (rigidbody_state_list_[k].isFeedbackNomral != feedback_state_temp) {
                    is_feedback_state_changed = true;
                }
                
                pub_list_[k]->PublishData(rigidbody_state_list_[k]);
            }
        }

        // If optitrack state changes, print the current info
        if (is_feedback_state_changed) {
            RCLCPP_INFO(this->get_logger(), "-----------------------------");
            if (is_feedback_all_good) {
                RCLCPP_INFO(this->get_logger(), "FEEDBACK NORMAL. RECEIVEING VRPN FROM THE FOLLOWING TOPICS:");
            } else {
                RCLCPP_WARN(this->get_logger(), "FEEDBACK LOST!! RECEIVEING VRPN FROM THE FOLLOWING TOPICS:");
            }
            
            for (int i = 0; i < num_channels_; i++) {
                if (rigidbody_state_list_[i].isFeedbackNomral) {
                    RCLCPP_INFO(this->get_logger(), "NORMAL:  %s", topic_list_[i].str);
                } else {
                    RCLCPP_WARN(this->get_logger(), "NO DATA: %s", topic_list_[i].str);
                }
            }
        }
    }

    // Member variables
    std::vector<SubTopic> topic_list_;
    std::vector<std::unique_ptr<OptiTrackFeedBackRigidBody>> mocap_list_;
    std::vector<std::unique_ptr<OptiTrackPublisher>> pub_list_;
    std::vector<rigidbody_state> rigidbody_state_list_;
    std::vector<optitrack_broadcast::msg::Mocap> mocap_message_list_;
    
    int num_channels_;
    int notfeedbackcounter_;
    std::string msg_type_;
    std::shared_ptr<rclcpp::Rate> rate_;
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<OptiTrackBroadcastNode>(argc, argv);
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}