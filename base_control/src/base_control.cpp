#include <ros/ros.h>
#include <std_msgs/String.h>
#include <std_msgs/Empty.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/Vector3.h>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

#define BUFSIZE 112

using namespace std;
using namespace boost::asio;

unsigned char buf[BUFSIZE];
io_service iosev;
serial_port sp(iosev,"/dev/ttyS0");

bool isBegin = false;
bool isTheOneLinear = false;
bool isOtherOneLinear = false;
bool isTheOneAngle = false;
bool isOtherOneAngle = false;
bool isThirdOneAngle = false;
bool isTheAngular = false;

unsigned char LinearXArry[10] = {0};
unsigned char LinearYArry[10] = {0};
unsigned char AngleXArry[10]  = {0};
unsigned char AngleYArry[10]  = {0};
unsigned char AngleZArry[10]  = {0};
unsigned char AngularYawArry[10] = {0};

char linearxIndex    = 0;
char linearyIndex    = 0;
char anglexIndex     = 0;
char angleyIndex     = 0;
char anglezIndex     = 0;
char angularyawIndex = 0;

float linearx = 0;
float lineary = 0;
float anglex  = 0;
float angley  = 0;
float anglez  = 0;
float angularyaw = 0;

void ProcessBuf(/*unsigned char& BUF[]*/)
{
    int i;
    isBegin = false;
    for(i = 0;i < BUFSIZE;i++)
    {
         if(buf[i] == ',' && isBegin == false)
         {
            isBegin = true;
            isTheOneLinear= true;
            isOtherOneLinear = false;
            isTheOneAngle = false;
            isOtherOneAngle = false;
            isThirdOneAngle = false;
            isTheAngular = false;
            linearxIndex    = 0;
            linearyIndex    = 0;
            anglexIndex     = 0;
            angleyIndex     = 0;
            anglezIndex     = 0;
            angularyawIndex = 0;
            continue;
         }
         if(buf[i] == ':'  && isTheOneLinear == true)
         {
             isTheOneLinear = false;
             isOtherOneLinear = true;
             continue;
         }
         if(buf[i] == '!'  && isOtherOneLinear == true)
         {
             isOtherOneLinear = false;
             continue;
         }
         if(buf[i] == '$' && isBegin == true)
         {
            isTheOneAngle = true;
            continue;
         }
        if(buf[i] == ':' && isTheOneAngle == true)
        {
            isOtherOneAngle = true;
            isTheOneAngle = false;
            continue;
        }
        if(buf[i] == ':' && isOtherOneAngle == true)
        {
            isThirdOneAngle = true;
            isOtherOneAngle = false;
            continue;
        }
        if(buf[i] == '!' && isThirdOneAngle == true)
        {
            isThirdOneAngle = false;
            continue;
        }
        if(buf[i] == '^' && isBegin == true)
        {
            isTheAngular = true;
            continue;
        }
        if(buf[i] == '!' && isTheAngular == true)
        {
            isTheAngular = false;
            //over and turn to float
            linearx = atof((const char*)LinearXArry);
            lineary = atof((const char*)LinearYArry);
            anglex  = atof((const char*)AngleXArry);
            angley  = atof((const char*)AngleYArry);
            anglez  = atof((const char*)AngleZArry);
            angularyaw = atof((const char*)AngularYawArry);

            return;
        }



         if(isBegin == true && isTheOneLinear == true)
         {
            LinearXArry[linearxIndex] = buf[i];
            linearxIndex ++;
         }
         if(isBegin == true && isOtherOneLinear == true)
         {
             LinearYArry[linearyIndex] = buf[i];
             linearyIndex ++;
         }
         if(isBegin == true && isTheOneAngle == true)
         {
             AngleXArry[anglexIndex] = buf[i];
             anglexIndex++;
         }
         if(isBegin == true && isOtherOneAngle == true)
         {
            AngleYArry[angleyIndex] = buf[i];
            angleyIndex++;
         }
         if(isBegin == true && isThirdOneAngle == true)
         {
            AngleZArry[anglezIndex] = buf[i];
            anglezIndex++;
         }
         if(isBegin == true && isTheAngular == true)
         {
            AngularYawArry[angularyawIndex] = buf[i];
            angularyawIndex++;
         }

    }


}



void cmd_vel_callback(const geometry_msgs::Twist::ConstPtr& sentmsg)
{
    //ROS_INFO_STREAM("Writing to serial port:\n");
    //ROS_INFO_STREAM("linear.x = " << sentmsg->linear.x << std::endl);
    //ROS_INFO_STREAM("linear.y = " << sentmsg->linear.y << std::endl);
   // ROS_INFO_STREAM("angular.z = "<< sentmsg->angular.z << std::endl);
    std::stringstream ss;
    ss << '#' << sentmsg->linear.x << ':' << sentmsg->linear.y << ':' << sentmsg->angular.z << "!\r\n";
    //ROS_INFO_STREAM(""<<ss.str());
    write(sp,buffer(ss.str())); //send serial data
}

int main(int argc,char** argv)
{
     ros::init(argc,argv,"base_control");
     ros::NodeHandle n;

     ros::Publisher chatter_pub_two = n.advertise<geometry_msgs::Vector3>("AngleMsgfrom32",1000);
     ros::Publisher chatter_pub = n.advertise<geometry_msgs::Twist>("TwistMsgfrom32",1000);
     ros::Subscriber chatter_sub = n.subscribe("cmd_vel",1000,cmd_vel_callback);

     ros::Rate loop_rate(50);


     sp.set_option(serial_port::baud_rate(115200));
     sp.set_option(serial_port::flow_control());
     sp.set_option(serial_port::parity());
     sp.set_option(serial_port::stop_bits());
     sp.set_option(serial_port::character_size(8));

     while(ros::ok())
     {
           //read message
           read(sp,buffer(buf));
           string str(&buf[0],&buf[BUFSIZE-1]);
           std_msgs::String stringmsg;
           std::stringstream ss;
           ss << str;
           stringmsg.data = ss.str();
           //ROS_INFO("%s",stringmsg.data.c_str());

           //process message
           ProcessBuf();
           geometry_msgs::Twist Twistmsg;
           Twistmsg.linear.x = linearx;
           Twistmsg.linear.y = lineary;
           Twistmsg.angular.z= angularyaw;
           chatter_pub.publish(Twistmsg);


           geometry_msgs::Vector3 Anglemsg;
           Anglemsg.x = anglex;
           Anglemsg.y = angley;
           Anglemsg.z = anglez;
           chatter_pub_two.publish(Anglemsg);

           //wait
           ros::spinOnce();
           loop_rate.sleep();
     }
     iosev.run();
     return 0;
}

