# xbot_navi

## 简介

xbot_navi是用于重德智能XBot-U科研教学平台机器人的**导航定位**ROS程序包。

该程序包具有建图、定位、导航、规划以及服务功能实现等多种功能。



## 使用方法
### 建立地图

在机器人上运行

```
roslaunch xbot_navi build_map.launch
```

然后在作为ROS从机的PC上运行可视化程序：

```
roslaunch xbot_navi rviz_build_map.launch
```

或者也可以选择使用google开源的cartographer来建图：

```
roslaunch xbot_navi build_map_carto.launch
```



### SLAM导航程序

在机器人上运行

```
roslaunch xbot_navi demo.launch
```

或者使用cartographer:

```
roslaunch xbot_navi navi_carto.launch
```



### 服务功能实现

该服务功能将导航、人脸识别、语音对话集合成一个整体的服务。

用户需要修改配置文件param/kp.json和param/greet.json来管理关键点和人脸问候语。

运行可参考

```
roslaunch xbot_navi demo.launch
```



## 参考链接

- [ROSwiki xbot tutorials](<http://wiki.ros.org/Robots/Xbot/tutorial/cn>)
- [ROSwiki xbot_navi软件说明](http://wiki.ros.org/xbot_navi)
- [重德智能](https://www.droid.ac.cn/)
- [XBot-U机器人网站介绍](https://www.droid.ac.cn/xbot_u.html)
- [中国大学慕课-机器人操作系统入门](https://www.icourse163.org/course/0802ISCAS001-1002580008)

## 联系我们

**商务合作**：bd@droid.ac.cn

**技术咨询**：wangpeng@droid.ac.cn或添加微信:18046501051（注明XBot-U咨询）



