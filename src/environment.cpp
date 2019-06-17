#include <ros/ros.h>
#include <visualization_msgs/Marker.h>
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/PoseStamped.h>

using namespace std;

ros::Publisher marker_pub;
visualization_msgs::Marker marker;
void goalCB(const geometry_msgs::PoseStamped msg)
{
  marker.pose = msg.pose;
  marker.pose.position.z = -0.2;
  cout<<msg.pose<<endl;
  cout<<"=============================================================================="<<endl;

  while (marker_pub.getNumSubscribers() < 1)
  {
    if (!ros::ok())
    {
      return ;
    }
    ROS_WARN_ONCE("Please create a subscriber to the marker");
    sleep(1);
  }
  marker_pub.publish(marker);

}

int main( int argc, char** argv )
{
  ros::init(argc, argv, "basic_shapes");
  ros::NodeHandle n;
  ros::Rate r(1);

  marker_pub = n.advertise<visualization_msgs::Marker>("visualization_marker", 1);

  ros::Subscriber goal_sub = n.subscribe("/goal",100, &goalCB);
  // Set the frame ID and timestamp.  See the TF tutorials for information on these.
  marker.header.frame_id = "/map";
  marker.header.stamp = ros::Time::now();

  // Set the namespace and id for this marker.  This serves to create a unique ID
  // Any marker sent with the same namespace and id will overwrite the old one
  marker.ns = "basic_shapes";
  marker.id = 0;

  // Set the marker type.  Initially this is CUBE, and cycles between that and SPHERE, ARROW, and CYLINDER
  marker.type = 10;
  marker.mesh_resource = "package://xbot_s/model/sdzn_full/saidi.DAE";
  marker.mesh_use_embedded_materials = true;

  // Set the marker action.  Options are ADD, DELETE, and new in ROS Indigo: 3 (DELETEALL)
  marker.action = visualization_msgs::Marker::ADD;

  // Set the pose of the marker.  This is a full 6DOF pose relative to the frame/time specified in the header

  // Set the scale of the marker -- 1x1x1 here means 1m on a side
  marker.scale.x = 4.5/3.2;
  marker.scale.y = 4.5/3.2;
  marker.scale.z = 4.5/3.2;

  // Set the color -- be sure to set alpha to something non-zero!
//    marker.color.r = 0.0f;
//    marker.color.g = 1.0f;
  marker.color.b = .5f;
  marker.color.a = 0.5;

  marker.lifetime = ros::Duration();

  // Publish the marker
  ros::spin();

//  while (ros::ok())
//  {

//    r.sleep();
//  }
}
