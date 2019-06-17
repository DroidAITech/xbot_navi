#include "ros/ros.h"
#include "std_msgs/UInt32.h"

int main(int argc, char **argv)
{
    ros::init(argc, argv, "auto_launch");
    ros::NodeHandle nh;

    ros::Publisher auto_launch_pub = nh.advertise<std_msgs::UInt32>("/office/next_loop", 1000);

    ros::Rate loop_rate(1);
    int wait_time = 255+60;
    while (wait_time >254)
    {
        std_msgs::UInt32 msg;
        msg.data = wait_time;
        auto_launch_pub.publish(msg);
        ros::spinOnce();
        wait_time--;

        loop_rate.sleep();
    }

    return 0;
}
