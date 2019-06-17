/*************************************************************************
	> File Name: auto_dock.h
	> Author:Rocwang 
	> Mail: yowlings@gmail.com; Github:https://github.com/yowlings
	> Created Time: 2018年06月04日 星期一 14时41分20秒
 ************************************************************************/

#ifndef _AUTO_DOCK_H
#define _AUTO_DOCK_H
#include "ros/ros.h"
#include <map>
#include <vector>
#include <math.h>
#include "sensor_msgs/LaserScan.h"
#include "xbot_navi/AutodockState.h"
#include "std_msgs/Bool.h"
#include "geometry_msgs/TwistStamped.h"

#include "boost/thread.hpp"
using namespace std;
namespace xbot {

class AutoDock {
public:
    AutoDock();
    ~AutoDock();


    void init();
    void laser_scanCB(const sensor_msgs::LaserScan);
    void launch_autodockCB(const std_msgs::Bool);
    void pub_autodock_state();
    void cal_docker_pose();
    void search_docker();
    void heading_docker();
    bool is_vertical();
    void go_direct();
    void go_middlepose();



private:
    float theta[3];
    float distance[3];
    int p[3];
    float theta_diff;
    float vertical_theta_thresh;
    bool found_docker;
    bool get_middlepose;
    bool reached_docker;
    ros::Subscriber laser_scan_sub;
    ros::Subscriber launch_autodock_sub;
    ros::Publisher xbot_cmd_vel_pub;
    ros::Publisher autodock_state_pub;
    xbot_navi::AutodockState autodock_state_msg;
    geometry_msgs::Twist cmd_vel_msg;


    boost::thread* state_pub_thread;
    boost::thread* search_dock_thread;
    boost::thread* heading_docker_thread;

};

}

#endif
