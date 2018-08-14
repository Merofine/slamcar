#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "geometry_msgs/Vector3.h"
#include "geometry_msgs/Pose.h"
#include "nav_msgs/Odometry.h"
#include <std_msgs/String.h>
#include <tf/transform_broadcaster.h>
#include "geometry_msgs/TransformStamped.h"
#include "geometry_msgs/PoseWithCovarianceStamped.h"
#include "geometry_msgs/Pose2D.h"

nav_msgs::Odometry odom_msgs;
//geometry_msgs::PoseWithCovarianceStamped pose_msgs;
geometry_msgs::TransformStamped odom_trans;
geometry_msgs::Quaternion odom_quat;
ros::Time current_time, last_time;
ros::Publisher  pub_3;
//ros::Publisher  pub_1;
double th = 0.0;
float x = 0;
float y = 0;


//接收到订阅的消息后，会进入消息回调函数
void TwistMsgfrom32Callback(const geometry_msgs::Twist::ConstPtr& msg)
{
//linear,angular
        odom_msgs.twist.twist.linear.x = msg->linear.x;
        odom_msgs.twist.twist.linear.y = msg->linear.y;
        odom_msgs.twist.twist.linear.z = 0;

        odom_msgs.twist.twist.angular.x = 0;
        odom_msgs.twist.twist.angular.y = 0;
        odom_msgs.twist.twist.angular.z = msg->angular.z;

//position,quaternion
/*
	double vth = msg->angular.z;

	current_time = ros::Time::now();

	double dt = (current_time - last_time).toSec();
	double delta_x = (odom_msgs.twist.twist.linear.x * cos(th) - odom_msgs.twist.twist.linear.y * sin(th)) * dt;
	double delta_y = (odom_msgs.twist.twist.linear.x * sin(th) + odom_msgs.twist.twist.linear.y * cos(th)) * dt;
	double delta_th= vth * dt;

	x += delta_x;
	y += delta_y;
	th+= delta_th;
		
	odom_msgs.pose.pose.position.x = x;
	odom_msgs.pose.pose.position.y = y;
	odom_msgs.pose.pose.position.z = 0.0;
	odom_quat = tf::createQuaternionMsgFromYaw(th);
	odom_msgs.pose.pose.orientation = odom_quat;

	pose_msgs.pose.pose.position.x = 0;//x;
	pose_msgs.pose.pose.position.y = 0;//y;
	pose_msgs.pose.pose.position.z = 0.0;
	odom_quat = tf::createQuaternionMsgFromYaw(0);//th
	pose_msgs.pose.pose.orientation = odom_quat;
	
	odom_msgs.header.stamp = current_time;
	pose_msgs.header.stamp = current_time;
	pub_3.publish(odom_msgs);
	pub_1.publish(pose_msgs);
	last_time = current_time;
*/
	//将接收到的消息打印出来
/*
	ROS_INFO("I heard linear.x: [%f]", msg->linear.x);
	ROS_INFO("I heard linear.y: [%f]", msg->linear.y);
        ROS_INFO("I heard angular.z: [%f]", msg->angular.z);  
*/
}

void AngleMsgfrom32Callback(const geometry_msgs::Vector3::ConstPtr& msg)
{
/*
	odom_quat = tf::createQuaternionMsgFromYaw(msg->z);

	odom_msgs.pose.pose.orientation = odom_quat;

	th = msg->z;
*/
	//将接收到的消息打印出来
	//ROS_INFO("I heard x: [%f]", msg->x);
	//ROS_INFO("I heard y: [%f]", msg->y);
	//ROS_INFO("I heard z: [%f]", msg->z);
}

void PoseMsgCallback(const geometry_msgs::Pose2D::ConstPtr& msg)	
{
	odom_msgs.pose.pose.position.x = msg->x;
	odom_msgs.pose.pose.position.y = msg->y;
	odom_msgs.pose.pose.position.z = 0;
	odom_quat = tf::createQuaternionMsgFromYaw(msg->theta);
	odom_msgs.pose.pose.orientation = odom_quat;

	//pose_msgs.pose.pose.position.x = 0;//x;
	//pose_msgs.pose.pose.position.y = 0;//y;
	//pose_msgs.pose.pose.position.z = 0.0;
	//odom_quat = tf::createQuaternionMsgFromYaw(0);//th
	//pose_msgs.pose.pose.orientation = odom_quat;

	current_time = ros::Time::now();
	odom_msgs.header.stamp = current_time;
	//pose_msgs.header.stamp = current_time;

	//pub_1.publish(pose_msgs);
	pub_3.publish(odom_msgs);
}

int main(int argc, char **argv)
{
	//初始化ROS节点
	ros::init(argc,argv,"odometry_car");

	//创建节点句柄
	ros::NodeHandle n;

	//创建一个Subscriber,订阅名为chatter的topic，注册回调函数chatterCallback
	//ros::Subscriber sub_1 = n.subscribe("AngleMsgfrom32", 1000, AngleMsgfrom32Callback);
	ros::Subscriber sub_2 = n.subscribe("TwistMsgfrom32", 1000, TwistMsgfrom32Callback);
	ros::Subscriber sub_3 = n.subscribe("pose2D",1000,PoseMsgCallback);


	pub_3 = n.advertise<nav_msgs::Odometry>("odom",1000);
	odom_msgs.header.frame_id = "odom";
	odom_msgs.child_frame_id = "base_link";

	//pub_1 = n.advertise<geometry_msgs::PoseWithCovarianceStamped>("initialpose",1000);
	//pose_msgs.header.frame_id = "map";

	ros::Rate loop_rate(50);

	//double x = 0.0;
	//double y = 0.0;

	//current_time = ros::Time::now();
	//last_time = ros::Time::now();

	while(ros::ok())
	{
		ros::spinOnce();
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
