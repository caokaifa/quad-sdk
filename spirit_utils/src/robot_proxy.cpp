#include "spirit_utils/robot_proxy.h"

RobotProxy::RobotProxy(ros::NodeHandle nh) {
  nh_ = nh;

  // Load rosparams from parameter server
  std::string control_input_topic, joint_encoder_topic, imu_topic, mocap_topic;
  nh.param<std::string>("topics/control_input", control_input_topic, "/control_input");
  nh.param<std::string>("topics/joint_encoder", joint_encoder_topic, "/joint_encoder");
  nh.param<std::string>("topics/imu", imu_topic, "/imu");
  nh.param<std::string>("topics/mocap", mocap_topic, "/mocap_optitrack/spirit/pose");
  nh.param<double>("robot_proxy/update_rate", update_rate_, 2);


  // Setup pubs and subs
  control_input_sub_ = nh_.subscribe(control_input_topic,1,&RobotProxy::controlInputCallback, this);
  joint_encoder_pub_ = nh_.advertise<sensor_msgs::JointState>(joint_encoder_topic,1);
  imu_pub_ = nh_.advertise<sensor_msgs::Imu>(imu_topic,1);
  mocap_pub_ = nh_.advertise<geometry_msgs::PoseStamped>(mocap_topic,1);
}

void RobotProxy::controlInputCallback(const spirit_msgs::ControlInput::ConstPtr& msg) {
}

void RobotProxy::publishJointEncoders() {
  sensor_msgs::JointState msg;

  std::vector<std::string> joint_names = {"8", "0", "1", "9","2", "3", "10", "4","5", "11", "6", "7"};
  std::vector<double> joint_pos (12,0);
  std::vector<double> joint_vel (12,0);
  std::vector<double> joint_effort (12,0);

  msg.name = joint_names;
  msg.position = joint_pos;
  msg.velocity = joint_vel;
  msg.effort = joint_effort;
  msg.header.stamp = ros::Time::now();
  joint_encoder_pub_.publish(msg);
}

void RobotProxy::publishImu() {
  sensor_msgs::Imu msg;
  msg.orientation.x = 0;
  msg.orientation.y = 0;
  msg.orientation.z = 0;
  msg.orientation.w = 1;
  imu_pub_.publish(msg);
}

void RobotProxy::publishMocap() {
  geometry_msgs::PoseStamped msg;
  msg.header.stamp = ros::Time::now();
  msg.pose.position.x = 0;
  msg.pose.position.y = 0;
  msg.pose.position.z = 0.3;

  double secs =ros::Time::now().toSec();
  double amplitude = 1.0;
  double period = 5;
  // msg.pose.position.y = amplitude*sin(2*3.14159*secs/period);

  msg.pose.orientation.x = 0;
  msg.pose.orientation.y = 0;
  msg.pose.orientation.z = 0;
  msg.pose.orientation.w = 1;
  mocap_pub_.publish(msg);
}

void RobotProxy::spin() {
  ros::Rate r(update_rate_);
  while (ros::ok()) {

    // Publish sensor data
    publishJointEncoders();
    publishImu();
    publishMocap();
    
    // Enforce update rate
    ros::spinOnce();
    r.sleep();
  }
}