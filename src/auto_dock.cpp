/*************************************************************************
	> File Name: src/auto_dock.cpp
	> Author: 
	> Mail: 
	> Created Time: 2018年05月31日 星期四 17时08分21秒
 ************************************************************************/

#include<iostream>
#include "../include/auto_dock.h"
#define MAX(x,y) (x>y)?y:x



namespace xbot{

AutoDock::AutoDock():theta_diff(100.0),vertical_theta_thresh(0.0),found_docker(false),get_middlepose(false),
reached_docker(false){

}

AutoDock::~AutoDock(){
    if(state_pub_thread){
        state_pub_thread->join();
        delete state_pub_thread;
    }
    if(search_dock_thread){
        search_dock_thread->join();
        delete search_dock_thread;
    }

}
void AutoDock::init(){
    ros::NodeHandle nh("~");
    laser_scan_sub = nh.subscribe("/scan",10,&AutoDock::laser_scanCB,this);
    p[0]=-1;
    p[1]=-1;
    p[2]=-1;
    launch_autodock_sub = nh.subscribe("/auto_dock/launch",10,&AutoDock::launch_autodockCB,this);
    autodock_state_pub = nh.advertise <xbot_navi::AutodockState> ("state",100);
    xbot_cmd_vel_pub = nh.advertise <geometry_msgs::Twist> ("/cmd_vel_mux/input/auto_dock",100);
    autodock_state_msg.header.stamp = ros::Time::now();
    autodock_state_msg.state = xbot_navi::AutodockState::NOT_LAUNCHED;



    state_pub_thread = new boost::thread(boost::bind(&AutoDock::pub_autodock_state,this));
    search_dock_thread = new boost::thread(boost::bind(&AutoDock::search_docker,this));
    heading_docker_thread = new boost::thread(boost::bind(&AutoDock::heading_docker,this));




}
void AutoDock::pub_autodock_state()
{
    ros::Rate autodock_state_rate(10);
    while(ros::ok())
    {
        autodock_state_pub.publish(autodock_state_msg);
        autodock_state_rate.sleep();


    }
}
void AutoDock::cal_docker_pose(){

}

bool AutoDock::is_vertical(){
//    机器人正对中心点,并且左右两边距离相等,形成等腰三角形,即垂直正对中心点
    return (abs(theta[1])<2*vertical_theta_thresh)&&(abs(distance[0]-distance[2])<0.05);
}

void AutoDock::search_docker(){
    ros::Rate search_rate(20);
    int search_times=0;
    ROS_INFO("pose1:%d;%d",p[0],p[2]);

    while((p[0]==-1||p[2]==-1)&&search_times<60*20/*&&!is_vertical()||abs(theta_diff)>0.004*/)
    {
//       ROS_INFO("pose1:%d;%d",p[0],p[2]);

        cmd_vel_msg.angular.z = 0.157;
        xbot_cmd_vel_pub.publish(cmd_vel_msg);
        search_rate.sleep();
        search_times++;
//        ROS_INFO("p[1]=%d;vertical:%d;theta_diff=%f",p[1],is_vertical(),theta_diff);

    }
    if(search_times>=60*20){
        ROS_INFO("searched for 30 seconds for turn 3 loops,but not found docker,please recheck!");
        found_docker=false;
        autodock_state_msg.state=xbot_navi::AutodockState::NOT_FOUND_DOCKER;

    }
    else{
        found_docker=true;

//        if(!is_vertical()){
//            get_middlepose=false;
//            search_times=0;
//            while(abs(theta_diff)>0.004&&search_times<60){
//                cmd_vel_msg.angular.z = 0.314;
//                xbot_cmd_vel_pub.publish(cmd_vel_msg);
//                search_rate.sleep();
//                search_times++;
//            }
//            if(search_times>=60){
//                ROS_INFO("searched for 60 seconds for turn 3 loops,but not found horizonal pose,please recheck!");
//                found_docker=false;
//                autodock_state_msg.state=xbot_navi::AutodockState::NOT_FOUND_DOCKER;

//            }
//        }
//        else{
//            get_middlepose=true;
//        }

    }

}
void AutoDock::go_direct(){
    ros::Rate forward_rate(20);
    while(distance[1]>0.2){
        cmd_vel_msg.linear.x=0.1;
        xbot_cmd_vel_pub.publish(cmd_vel_msg);
        forward_rate.sleep();
    }



    reached_docker=true;
}
void AutoDock::go_middlepose(){
    ros::Rate forward_rate(20);
    while(p[0]>5&&abs(p[2]-524)<=5){
        cmd_vel_msg.linear.x=0.1;
        xbot_cmd_vel_pub.publish(cmd_vel_msg);
        forward_rate.sleep();
    }
    int times=0;
    while(!is_vertical()&&times<60){
        cmd_vel_msg.angular.z=0.314;
        xbot_cmd_vel_pub.publish(cmd_vel_msg);
        forward_rate.sleep();
    }
    if(times<60)
    {
        get_middlepose=true;
    }

}
void AutoDock::heading_docker(){
    ros::Rate cmd_pub_rate(20);
    while(ros::ok()&&!reached_docker){
        if(found_docker){
            autodock_state_msg.state=xbot_navi::AutodockState::HEADING;
            cmd_vel_msg.angular.z=MAX(0.2*(theta[0]+theta[2])/2-(distance[0]-distance[2]),1.0);
            cmd_vel_msg.linear.x=MAX(0.1*(distance[0]+distance[2])/2,0.5);
            xbot_cmd_vel_pub.publish(cmd_vel_msg);
            cmd_pub_rate.sleep();

//            if(get_middlepose=true){
//                go_direct();

//            }
//            else{
//                go_middlepose();
//            }

        }
    }

}

void AutoDock::launch_autodockCB(const std_msgs::Bool msg){
    if(msg.data == true){
        if(autodock_state_msg.state%2==0){
            ROS_INFO("launch autodock success, start searching docker around robot.");
            autodock_state_msg.state = xbot_navi::AutodockState::SEARCHING;


        }
        else{
            ROS_INFO("xbot autodock has already launched, please check /auto_dock/state for autodock state and try later!");
        }
    }

}
void AutoDock::laser_scanCB(const sensor_msgs::LaserScan msg){
    vertical_theta_thresh = msg.angle_increment;
    int length  = msg.ranges.size();
    vector<float> a = msg.intensities;
    int i=0,j=0;
    p[0]=-1;
    for(;i<a.size();i++)
    {
        if(a[i]>200)
        {
            for(;j<50;j++)
            {
                if(a[i+j]<200) break;
            }
            if(j>=20)
            {
                p[0]=i;
                break;
            }

        }

    }
    i=i+j;
    j=0;
    p[2]=-1;
    for(;i<a.size();i++)
    {
        if(a[i]>200)
        {
            for(;j<50;j++)
            {
                if(a[i+j]<200) break;
            }
            if(j>=20)
            {
                p[2]=i+j;
                break;
            }

        }

    }



    p[1]=-1;
    if(p[0]!=-1&&p[2]!=-1)
    {
        p[1] = (p[0]+p[2])/2;
    }

    if(p[1]!=-1){
        for(int pii=0;pii<3;pii++){
            theta[pii]=msg.angle_min+p[pii]*msg.angle_increment;
            distance[pii]=msg.ranges[p[pii]];
        }
        theta_diff = abs(tan(theta[1]))-(abs(tan(theta[0]))+abs(tan(theta[2])))/2;

    }
    else{
        theta_diff = 100;
    }



//    ROS_INFO("p[1]=%d;vertical:%d;theta_diff=%f",p[1],is_vertical(),theta_diff);
    if(abs(theta_diff)<0.004)
    {
        ROS_INFO("found right direction!");
    }


    ROS_INFO("pose0:%d; pose2: %d",p[0],p[2]);
//    ROS_INFO("docker pose:%d/%d",p[0],length);

}



}

/*****************************************************************************
** Main
*****************************************************************************/



int main(int argc, char **argv)
{
  ros::init(argc, argv, "auto_dock");
  xbot::AutoDock autodock;
  autodock.init();
  ros::spin();
  return 0;
}

