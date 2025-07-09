from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        # Server IP argument
        DeclareLaunchArgument(
            'server',
            default_value='192.168.2.230',
            description='VRPN server IP address'
        ),

        # VRPN Client Node
        Node(
            package='vrpn_client_ros',
            executable='vrpn_client_node',
            name='vrpn_client_node',
            parameters=[{
                'server': LaunchConfiguration('server'),
                'port': 3883,
                'update_frequency': 100.0,
                'frame_id': 'world',
                'use_server_time': False,
                'broadcast_tf': True,
                'refresh_tracker_frequency': 1.0,
                # 'trackers': ['FirstTracker', 'SecondTracker']
            }],
            output='screen'
        ),

        # Optitrack Broadcast Node
        Node(
            package='optitrack_broadcast',
            executable='broadcast_position_velocity',
            name='broadcast_1',
            output='screen',
            arguments=['UAV', 'cf1', 'cf2', 'cf3', 'cf4', 'cf5']
        )
    ])