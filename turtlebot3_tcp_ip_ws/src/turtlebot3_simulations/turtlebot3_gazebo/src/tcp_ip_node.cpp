#include "ros/ros.h"
#include "math.h"
#include "std_msgs/String.h"
#include "std_msgs/Float64.h"
#include "sensor_msgs/Imu.h"
#include "nav_msgs/Odometry.h"
#include <iostream>
#include <turtlebot3_msgs/LandmarkDistance.h>
#include <turtlebot3_msgs/SensorState.h>

class Landmark
{
    public:
        std::string name;
        double x;
        double y;

        Landmark(std::string name, double x, double y)
            : name(name), x(x), y(y)
            {

            }
};

class LandmarkMonitor
{
    private:
        std::vector<Landmark> landmarks;
        ros::Publisher landmark_pub;

        void InitLandmarks()
        {
            landmarks.push_back(Landmark("Cylinder_1", 1.00, 1.00));
            landmarks.push_back(Landmark("Cylinder_2", 1.00, 0.00));
            landmarks.push_back(Landmark("Cylinder_3", 1.00, -1.00));
            landmarks.push_back(Landmark("Cylinder_4", 0.00, 1.00));
            landmarks.push_back(Landmark("Cylinder_5", 0.00, 0.00));
            landmarks.push_back(Landmark("Cylinder_6", 0.00, -1.00));
            landmarks.push_back(Landmark("Cylinder_7",-1.00, 1.00));
            landmarks.push_back(Landmark("Cylinder_8",-1.00, 0.00));
            landmarks.push_back(Landmark("Cylinder_9",-1.00, -1.00));
        }
        turtlebot3_msgs::LandmarkDistance FindClosestLandmark(double x, double y)
        {
            turtlebot3_msgs::LandmarkDistance result;
            result.distance = -1;

            for(int i = 0; i < landmarks.size(); i++)
            {
                const Landmark& landmark = landmarks[i];
                double x_distance = landmark.x - x;
                double y_distance = landmark.y - y;
                // double distance = sqrt(pow(x_distance, 2) - (y_distance, 2));
                double distance = sqrt(abs(x_distance * x_distance - y_distance * y_distance));

                if(result.distance < 0 || distance < result.distance)
                {
                    result.name = landmark.name;
                    result.distance = distance;
                }
            }

            return result;
        }
    public:
        void odomCallBack(const nav_msgs::Odometry::ConstPtr& msg)
        {
            double x = msg -> pose.pose.position.x;
            double y = msg -> pose.pose.position.y;
            ROS_INFO("x: %f y: %f", x, y);
            turtlebot3_msgs::LandmarkDistance closestLandmark = FindClosestLandmark(x, y);
            // ROS_INFO("name: %s, distance: %f", closestLandmark.name.c_str(), closestLandmark.distance);
            landmark_pub.publish(closestLandmark);
        }


        LandmarkMonitor(const ros::Publisher& landmak_pub): landmarks(), landmark_pub(landmak_pub)
        {
            InitLandmarks();
        }

};

int main(int argc, char **argv)
{
    ros::init(argc, argv, "tcp_ip_node");

    ros::NodeHandle nh;

    ros::Publisher landmark_pub = nh.advertise<turtlebot3_msgs::LandmarkDistance>("closest_landmark", 100);

    LandmarkMonitor monitor(landmark_pub);

    ros::Subscriber sub = nh.subscribe("odom", 10, &LandmarkMonitor::odomCallBack, &monitor);

    ros::spin();

    return 0;
}