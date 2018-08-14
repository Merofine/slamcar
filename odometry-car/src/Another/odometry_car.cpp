#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "geometry_msgs/Vector3.h"
#include "geometry_msgs/Pose.h"
#include "nav_msgs/Odometry.h"
#include <std_msgs/String.h>
#include <tf/transform_broadcaster.h>
#include "geometry_msgs/TransformStamped.h"
#include "geometry_msgs/PoseWithCovarianceStamped.h"
#include <math.h>

#define PI 3.14159265358979323846

nav_msgs::Odometry odom_msgs;
geometry_msgs::PoseWithCovarianceStamped pose_msgs;
geometry_msgs::TransformStamped odom_trans;
float Yaw = 0;
float q0  = 0;
float q1  = 0;
float q2  = 0;
float q3  = 0;


//接收到订阅的消息后，会进入消息回调函数
void TwistMsgfrom32Callback(const geometry_msgs::Twist::ConstPtr& msg)
{
        odom_msgs.twist.twist.linear.x = msg->linear.x;
        odom_msgs.twist.twist.linear.y = msg->linear.y;
        odom_msgs.twist.twist.linear.z = 0;

        odom_msgs.twist.twist.angular.x = 0;
        odom_msgs.twist.twist.angular.y = 0;
        odom_msgs.twist.twist.angular.z = msg->angular.z;

	//将接收到的消息打印出来
	ROS_INFO("I heard linear.x: [%f]", msg->linear.x);
	ROS_INFO("I heard linear.y: [%f]", msg->linear.y);
        ROS_INFO("I heard angular.z: [%f]", msg->angular.z);  
}

void AngleMsgfrom32Callback(const geometry_msgs::Vector3::ConstPtr& msg)
{
	//geometry_msgs::Quaternion odom_quat = tf::createQuaternionMsgFromYaw(msg->z);
	//odom_msgs.pose.pose.orientation = odom_quat;
	
	//将接收到的消息打印出来
	ROS_INFO("I heard x: [%f]", msg->x);
	ROS_INFO("I heard y: [%f]", msg->y);
	ROS_INFO("I heard z: [%f]", msg->z);
}	


void OdomfromzedCallback(const nav_msgs::Odometry::ConstPtr& msg)
{
	ROS_INFO("I heard zed_odom_fromzed");
	odom_msgs.pose.pose.position.x = msg->pose.pose.position.x;
        odom_msgs.pose.pose.position.y = msg->pose.pose.position.y;
	odom_msgs.pose.pose.position.z = 0;

	pose_msgs.pose.pose.position.x = odom_msgs.pose.pose.position.x;
	pose_msgs.pose.pose.position.y = odom_msgs.pose.pose.position.y;
	pose_msgs.pose.pose.position.z = 0;

	q1 = msg->pose.pose.orientation.x;
	q2 = msg->pose.pose.orientation.y;
	q3 = msg->pose.pose.orientation.z;
	q0 = msg->pose.pose.orientation.w;
	Yaw = atan2(2*(q1*q2+q0*q3),q0*q0+q1*q1-q2*q2-q3*q3)*57.3;
	Yaw = Yaw*PI/180;	
	geometry_msgs::Quaternion odom_quat = tf::createQuaternionMsgFromYaw(Yaw);
	odom_msgs.pose.pose.orientation = odom_quat;
	pose_msgs.pose.pose.orientation = odom_quat;
	//sent tf

/*
	tf::TransformBroadcaster odom_broadcaster;
  	ros::Time current_time;
	current_time = ros::Time::now();
 	odom_trans.header.stamp = current_time;
        odom_trans.header.frame_id = "odom";
        odom_trans.child_frame_id = "base_link";
 
     	odom_trans.transform.translation.x = 0;//msg->pose.pose.position.x;
    	odom_trans.transform.translation.y = 0;msg->pose.pose.position.y;
    	odom_trans.transform.translation.z = 0.0;
    	odom_trans.transform.rotation.w =1; //odom_msgs.pose.pose.orientation;
	odom_broadcaster.sendTransform(odom_trans);
*/
}



int main(int argc, char **argv)
{
	//初始化ROS节点
	ros::init(argc,argv,"odometry_car");

	//创建节点句柄
	ros::NodeHandle n;

	//创建一个Subscriber,订阅名为chatter的topic，注册回调函数chatterCallback
	ros::Subscriber sub_1 = n.subscribe("AngleMsgfrom32", 1000, AngleMsgfrom32Callback);
	ros::Subscriber sub_2 = n.subscribe("TwistMsgfrom32", 1000, TwistMsgfrom32Callback);

	ros::Subscriber sub_3 = n.subscribe("zed_odom", 1000, OdomfromzedCallback);
	ros::Publisher  pub_3 = n.advertise<nav_msgs::Odometry>("odom",1000);

	ros::Publisher  pub_1 = n.advertise<geometry_msgs::PoseWithCovarianceStamped>("initialpose",1000);
	ros::Rate loop_rate(50);

	ros::Time current_time;
	current_time = ros::Time::now();
	odom_msgs.header.stamp = current_time;
	odom_msgs.header.frame_id = "odom";
	odom_msgs.child_frame_id = "base_link";

	pose_msgs.header.stamp = current_time;
	pose_msgs.header.frame_id = "map";
	
	while(ros::ok())
	{
		ros::spinOnce();
		pub_3.publish(odom_msgs);
		pub_1.publish(pose_msgs);
		loop_rate.sleep();
		
	}
	//开三线程
	//ros::AsyncSpinner spinner(3);
	//spinner.start();
	//ros::waitForShutdown();

	//waiting for the callback
	//ros::spin();

	return 0;
}
