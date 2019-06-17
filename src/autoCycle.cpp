
#include "ros/ros.h"
#include "std_msgs/String.h"

int main(int argc, char **argv)
{
    ros::init(argc, argv, "auto_launch");
    ros::NodeHandle nh;

    ros::Publisher auto_cycle_pub = nh.advertise<std_msgs::String>("/office/goal_name", 1000);

    ros::Rate loop_rate(1);
    int wait_time = 10;
    while (wait_time)
    {
        std_msgs::String msg;
        if(wait_time > 1)
        {
            msg.data = "nothing";
        }
        else{
            msg.data = "A";
        }
        auto_cycle_pub.publish(msg);
        wait_time--;

        loop_rate.sleep();
    }

    return 0;
}
