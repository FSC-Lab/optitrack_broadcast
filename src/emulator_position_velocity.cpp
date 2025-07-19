#include <memory>
#include <iostream>
#include <vector>
#include <string>
#include <cstring>

#include "rclcpp/rclcpp.hpp"
#include "Eigen/Eigen"
#include "Mocap_emulator.hpp"

struct SubTopic
{
    char str[100];
};
struct PubTopic
{
    char str[100];
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    
    // Create node with a unique name
    auto node = std::make_shared<rclcpp::Node>("emulator_position_velocity");
    
    // Frequency parameter
    double frequency = 100.0;
    node->declare_parameter("frequency", frequency);
    frequency = node->get_parameter("frequency").as_double();

    /* mocap processor instances */
    std::vector<std::unique_ptr<Mocap_emulator>> EmulatorList;

    int NumOfChannels = argc - 1;
    
    // must have at least 1 input argument for topic name
    if (argc > 1) {
        RCLCPP_INFO(node->get_logger(), "++++++++RECEIVEING GAZEBO PLUGIN INFO FROM THE FOLLOWING TOPICS:++++++++");
        
        for (int i = 1; i < argc; i++) {
            // SubTopic SubTopic;
            // PubTopic PubTopic;
            std::string subTopic = "/model/" + std::string(argv[i]) + "/odometry";
            std::string pubTopic = "/mocap/" + std::string(argv[i]);
            
            RCLCPP_INFO(node->get_logger(), "%s publish emulated data to: %s", 
                        subTopic, pubTopic);
            
            /* create an optitrack instance in the heap and push the pointer into the container */
            EmulatorList.emplace_back(
                std::make_unique<Mocap_emulator>(
                    pubTopic, 
                    subTopic, 
                    node, 
                    static_cast<int>(frequency)
                )
            );
        }
    } else {
        RCLCPP_WARN(node->get_logger(), "NO TOPIC NAME SPECIFIED!");
    }
    
    RCLCPP_INFO(node->get_logger(), "EMULATOR RUNNING...");

    //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>Main Loop<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
    rclcpp::spin(node);
    rclcpp::shutdown();
    
    return 0;
}