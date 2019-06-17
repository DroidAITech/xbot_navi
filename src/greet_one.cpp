#include "ros/ros.h"
#include "xbot_talker/play.h"
#include "xbot_talker/chat.h"
#include "xbot_face/FaceResult.h"
#include <string.h>


using namespace std;
int detect_times = 0;
bool meet = false;

ros::ServiceClient playclient;
ros::ServiceClient chatclient;

string kehu = "lyh";
string wenhou = "你好，刘银河";

void faceCB(xbot_face::FaceResult msg){
    if(meet)
    {
        return;
    }

    if(msg.face_exist == false)
    {
        detect_times = 0;
        return;
    }

    if((msg.name == kehu)&&(msg.confidence>0.6))
    {
        detect_times+=1;

    }
    else
    {
        detect_times = 0;
    }

    if (detect_times>=5)
    {
        meet = true;
        xbot_talker::play srv;
        srv.request.mode = 1;
        srv.request.tts_text = wenhou;

        if (playclient.call(srv))
        {
          //ROS_INFO("Result: %s", srv.response.result);
        }
        else
        {
          ROS_ERROR("Failed to call service play");
          return;
        }
        xbot_talker::chat srv1;
        srv1.request.start_chat = true;

        if(chatclient.call(srv1))
        {
            ROS_INFO("Result: %b", srv1.response.chat_success);
            exit(0);
        }
        else
        {
          ROS_ERROR("Failed to call service play");
          return;
        }

    }

}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "play_client");
  ros::NodeHandle n;


  ros::Subscriber sub = n.subscribe("/xbot/face_result",1000, faceCB);
  playclient = n.serviceClient<xbot_talker::play>("/play");
  chatclient = n.serviceClient<xbot_talker::chat>("/chat");
  ros::spin();



  return 0;
}

