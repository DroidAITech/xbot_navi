#!/usr/bin/env python
#coding=utf-8

import rospy, yaml, os, json,time

from xbot_face.msg import FaceResult
from xbot_talker.srv import chat, play
from std_msgs.msg import String, UInt32, UInt8, Bool
from geometry_msgs.msg import Pose, PoseStamped
from actionlib_msgs.msg import GoalStatusArray
from move_base_msgs.msg import MoveBaseActionResult
from std_srvs.srv import Empty

class demo():
	"""docstring for welcome"""
	def __init__(self):
#       声明节点订阅与发布的消息

		# 发布目标点信息
		self.move_base_goal_pub = rospy.Publisher('/move_base_simple/goal', PoseStamped, queue_size=1)
		# 订阅人脸识别结果
		self.face_result_sub = rospy.Subscriber('/xbot/face_result', FaceResult, self.faceCB)
		# 订阅是否到达目标点结果
		self.move_base_result_sub = rospy.Subscriber('/move_base/result', MoveBaseActionResult, self.move_base_resultCB)
		# 请求清除costmap服务
		self.clear_costmaps_srv = rospy.ServiceProxy('/move_base/clear_costmaps',Empty)
		# 订阅是否收到visit信息
		self.visit_sub = rospy.Subscriber('/demo/visit', Bool, self.visitCB)

		# 请求chat服务
		self.chat_srv = rospy.ServiceProxy('/chat',chat)
		# 请求播放服务
		self.play_srv = rospy.ServiceProxy('/play',play)


#        记录机器人当前的目标点
		self.current_goal = 0

		# self.VIP_name = 'wp'

		self.greet_last_time = time.time()

		self.getVIP = False
		self.known_face_times = 0
		self.unknown_face_times = 0

#        读取一存储的讲解点字典文件,默认位于xbot_s/param/position_dic.yaml文件
		self.kp_path = rospy.get_param('/demo/kp_path','/home/xbot/catkin_ws/src/xbot_navi/script/demo/standard/kp.json')
		self.greet_path = rospy.get_param('/demo/greet_path','/home/xbot/catkin_ws/src/xbot_navi/script/demo/standard/greet.json')
#		yaml_path = yaml_path + '/scripts/position_dic.yaml'
		with open(self.kp_path, 'r') as json_file:
			self.kp_list = json.load(json_file)
		json_file.close()

		with open(self.greet_path,'r') as f:
			self.greet_dict = json.load(f)
		f.close()
		rospy.spin()

	def greeting(self,name):
		if name == 'unknown':
			self.play_srv(False,1,'','您好，我还不认识您，我正在等待一位重要客人，请自行参观，如有需要请注册。')
		else:
			resp1 = self.play_srv(False,1,'',self.greet_dict[name]['greet_words'])
			if resp1 and self.greet_dict[name]['isVIP']:
				self.getVIP = True
				self.chat_srv(True)
				# self.pub_kp()



	def faceCB(self,msg):
		if not self.getVIP:
			if not msg.face_exist:
				self.known_face_times = 0
				self.unknown_face_times = 0
			else:
				if msg.confidence >0.55:
					self.known_face_times+=1
					self.unknown_face_times = 0
				else:
					self.unknown_face_times +=1
					self.known_face_times = 0


			if time.time()-self.greet_last_time>5:
				if self.known_face_times >=5:
					self.greeting(msg.name)
					self.known_face_times = 0

			
				if self.unknown_face_times >=10:
					self.greeting('unknown')
					self.unknown_face_times = 0
					
				self.greet_last_time = time.time()
		else:
			pass

	def pub_kp(self):
		if self.current_goal < len(self.kp_list):
			pos = self.kp_list[self.current_goal]['pose']
			goal = PoseStamped()
			goal.header.frame_id = 'map'
			goal.pose.position.x = pos[0][0]
			goal.pose.position.y = pos[0][1]
			goal.pose.position.z = pos[0][2]
			goal.pose.orientation.x = pos[1][0]
			goal.pose.orientation.y = pos[1][1]
			goal.pose.orientation.z = pos[1][2]
			goal.pose.orientation.w = pos[1][3]
			print goal
			self.move_base_goal_pub.publish(goal)

	def visitCB(self, msg):
		if msg.data:
			end_talk = self.play_srv(False, 1, '', '好的，您请跟我来！')
			if end_talk:
				self.pub_kp()


#    导航程序对前往目标点的执行结果
	def move_base_resultCB(self, result):
		if result.status.status == 3:
#            成功到达目标点
			kp = self.kp_list[self.current_goal]
			if kp['play']:
				resp = self.play_srv(False, 1, '', kp['play_words'])
				if resp:
					self.current_goal+=1
					self.clear_costmaps_srv()
					self.pub_kp()
			else:
				self.current_goal += 1
				self.clear_costmaps_srv()
				self.pub_kp()
		elif result.status.status == 4:
#			到达目标点失败,slam发布abort信号给talker,talker会请求前方人员让一下,然后重新规划路径尝试去往目标点
#           TODO:此时启用胸前深度摄像头确认前方障碍物情况再决定是否请求人员让一下
			self.pub_kp()




if __name__ == '__main__':
	rospy.init_node('demo_node')
	try:
		rospy.loginfo('demo node initialized...')
		demo()
	except rospy.ROSInterruptException:
		rospy.loginfo('demo node initialize failed, please retry...')
