from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        DeclareLaunchArgument(
            'objects',
            default_value='x500_0',
            description='List of objects to track'
        ),
        
        Node(
            package='optitrack_broadcast',
            executable='emulator_position_velocity',
            name='mocap_emulator',
            output='screen',
            arguments=LaunchConfiguration('objects').split()
        )
    ])