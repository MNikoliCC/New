#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <iostream>

#include "nlohmann/json.hpp"
#include <fstream>

#include "ros/ros.h"
#include "std_msgs/String.h"
#include "std_msgs/Float64.h"
#include "sensor_msgs/Imu.h"
#include "nav_msgs/Odometry.h"
#include <iostream>
#include <turtlebot3_msgs/LandmarkDistance.h>
#include <turtlebot3_msgs/SensorState.h>

const int PORT = 54000;

class TcpIpServer
{
    private:
        int listening;
        int clientSocket;
        int message = 1;
        bool rosfinished = false;
        std::string sMessage_1;
        std::string sMessage_2;
        std::string sMessage_3;
        std::string sMessage_4;
        std::string sMessage_final;
        nlohmann::json json_data_2;
        nlohmann::json json_data_3;
        nlohmann::json json_data_4;
        nlohmann::json json_data_final;
        ros::NodeHandle nh;
        ros::NodeHandle nh2;
        ros::Subscriber tcp_ip_client_sub = nh.subscribe("odom", 1, &TcpIpServer::robotDataUpdate, this);
        ros::Subscriber tcp_ip_client_sub_2 = nh.subscribe("closest_landmark", 1, &TcpIpServer::closestLandmark, this);

    public:
        nlohmann::json toJson(const char* jsonString)
        {
            nlohmann::json jsonObj;
            std::stringstream(jsonString) >> jsonObj;

            return jsonObj;
        }

        void generateJson()
        {
            std::cout << "Generating json files..." << std::endl;

            //Converting messages to json object
            json_data_final = nlohmann::json::parse(sMessage_1);
            //Generating final.josn file
            std::ofstream json_out_1("client_data_updated.json");
            //Write json data into the json file
            json_out_1 << std::setw(4) << json_data_final;
            std::cout << std::setw(4) << json_data_final << std::endl;
            
            //Converting messages to json object
            json_data_final = nlohmann::json::parse(sMessage_2);
            //Generating final.josn file
            std::ofstream json_out_2("robot_updated.json");
            //Write json data into the json file
            json_out_2 << std::setw(4) << json_data_final;
            std::cout << std::setw(4) << json_data_final << std::endl;

            //Converting messages to json object
            json_data_final = nlohmann::json::parse(sMessage_3);
            //Generating final.josn file
            std::ofstream json_out_3("closest_landmark_updated.json");
            //Write json data into the json file
            json_out_3 << std::setw(4) << json_data_final;
            std::cout << std::setw(4) << json_data_final << std::endl;

            //Converting messages to json object
            json_data_final = nlohmann::json::parse(sMessage_4);
            //Generating final.josn file
            std::ofstream json_out_4("goal_landmark_updated.json");
            //Write json data into the json file
            json_out_4 << std::setw(4) << json_data_final;
            std::cout << std::setw(4) << json_data_final << std::endl;
            
            // //Not pretty printed, but file consumes less space
            // json_out << json_data_final;
        }

        int createSocket()
        {
            // Create a socket

            std::cout << "Creating server socket..." << std::endl;
            listening = socket(AF_INET, SOCK_STREAM, 0);
            if (listening == -1)
            {
                std::cerr << "Can't create a socket!";
                return -1;
            }
            else
            {
                return 0;
            }
        }

        int bindSocketAndIp()
        {
            // Bind the ip address and port to a socket

            struct sockaddr_in hint;
            hint.sin_family = AF_INET;
            hint.sin_port = htons(PORT);
            inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);

            std::cout << "Binding socket to sockaddr..." << std::endl;
            if (bind(listening, (struct sockaddr *)&hint, sizeof(hint)) == -1) 
            {
                std::cerr << "Can't bind to IP/port" << std::endl;
                return -2;
            }
            else
            {
                return 0;
            }
        }

        int listenToTheSocket()
        {
            //Listen to the socket

            std::cout << "Mark the socket for listening..." << std::endl;
            if (listen(listening, SOMAXCONN) == -1)
            {
                std::cerr << "Can't listen !" << std::endl;
                return -3;
            }
            else
            {
                return 0;
            }
        }

        int acceptClientCall()
        {
            sockaddr_in client;
            socklen_t clientSize = sizeof(client);

            //Accept client call, and make a new socket to communicate

            std::cout << "Accept client call..." << std::endl;
            clientSocket = accept(listening, (struct sockaddr *)&client, &clientSize);

            std::cout << "Received call..." << std::endl;
            if (clientSocket == -1)
            {
                std::cerr << "Problem with client connecting!" << std::endl;
                return -4;
            }
            else
            {
                std::cout << "Client address: " << inet_ntoa(client.sin_addr) << " and port: " << client.sin_port << std::endl;

                std::ifstream file("client_data.json");
                json_data_2 = nlohmann::json::parse(file);
                json_data_2["Client data"]["IP ADDRESS"] = inet_ntoa(client.sin_addr);
                json_data_2["Client data"]["PORT"] = client.sin_port;

                close(listening);

                //Communicate
                std::cout << "Ready to communicate..." << std::endl;
                return 0;
            }
        }

        void communicate()
        {
            char buf[4096];
            bool flag = true;
            int bytesRecv;

            while (ros::ok())
            {
                if (message == 1)
                {
                    std::cout << "\nMESSAGE 1\n";
                    sMessage_1 = json_data_2.dump(4);
                    send(clientSocket, sMessage_1.c_str(), sMessage_1.size(), 0);
                    std::cout << "Status: SENT\n";

                    message ++;
                }

                if (message == 2)
                {
                    std::cout << "\nMESSAGE 2\n";
                    memset(buf, 0, 4096);
                    //Wait for a message
                    bytesRecv = recv(clientSocket, buf, 4096, 0);
                    if (bytesRecv == -1)
                    {
                        std::cerr << "There was a connection issue." << std::endl;
                    }
                    else if (bytesRecv == 0)
                    {
                        std::cout << "The client disconnected" << std::endl;
                    }
                    else
                    {
                        //Display message
                        std::cout << "CLIENT:\n" << std::string(buf, 0, bytesRecv) << std::endl;

                        //Converting to nlohmann json object
                        json_data_2 = toJson(buf);

                        //Clear buffer
                        memset(buf, 0, 4096);

                        //Updating json
                        ros::spinOnce();

                        // std::fflush(stdin);
                        // std::cin.clear();

                        if(rosfinished)
                        {
                            std::cout << "\nRobot data updated!" << std::endl;
                        
                            //Converting to string
                            sMessage_2 = json_data_2.dump(4);

                            send(clientSocket, sMessage_2.c_str(), sMessage_2.size(), 0);
                            std::cout << "Status: SENT\n";
                            message ++;
                            rosfinished = false;
                        }
                    }
                }

                if (message == 3)
                {
                    std::cout << "\nMESSAGE 3\n";
                    memset(buf, 0, 4096);
                    // Wait for a message
                    bytesRecv = recv(clientSocket, buf, 4096, 0);
                    if (bytesRecv == -1)
                    {
                        std::cerr << "There was a connection issue." << std::endl;
                    }
                    else if (bytesRecv == 0)
                    {
                        std::cout << "The client disconnected" << std::endl;
                    }
                    else
                    {
                        //Display message
                        std::cout << "CLIENT:\n" << std::string(buf, 0, bytesRecv) << std::endl;

                        //Converting to nlohmann json object
                        json_data_3 = toJson(buf);

                        //Updating json
                        while(!rosfinished)
                        {
                            ros::spinOnce();
                        }

                        if(rosfinished)
                        {
                            std::cout << "\nClosest landmark updated!" << std::endl;
                            
                            //Converting to string
                            sMessage_3 = json_data_3.dump(4);

                            send(clientSocket, sMessage_3.c_str(), sMessage_3.size(), 0);
                            std::cout << "Status: SENT\n";
                            message ++;
                            rosfinished = false;
                        }
                    }
                }

                if (message == 4)
                {
                    std::cout << "\nMESSAGE 4\n";
                    memset(buf, 0, 4096);
                    //Wait for a message
                    bytesRecv = recv(clientSocket, buf, 4096, 0);
                    if (bytesRecv == -1)
                    {
                        std::cerr << "There was a connection issue." << std::endl;
                    }
                    else if (bytesRecv == 0)
                    {
                        std::cout << "The client disconnected" << std::endl;
                    }
                    else
                    {
                        //Display message
                        std::cout << "CLIENT:\n" << std::string(buf, 0, bytesRecv) << std::endl;

                        //Converting to nlohmann json object
                        json_data_4 = toJson(buf);

                        //Updating json
                        while(!rosfinished)
                        {
                            ros::spinOnce();
                        }

                        if(rosfinished)
                        {
                            std::cout << "\nRobot reached the desired landmark!" << std::endl;
                            
                            //Converting to string
                            sMessage_4 = json_data_4.dump(4);

                            send(clientSocket, sMessage_4.c_str(), sMessage_4.size(), 0);
                            std::cout << "Status: SENT\n";
                            message ++;
                            rosfinished = false;
                        }
                    }
                }

                if (message == 5)
                {
                    std::cout << "\nMESSAGE 5\n";
                    memset(buf, 0, 4096);
                    // Wait for a message
                    bytesRecv = recv(clientSocket, buf, 4096, 0);
                    if (bytesRecv == -1)
                    {
                        std::cerr << "There was a connection issue." << std::endl;
                    }
                    else if (bytesRecv == 0)
                    {
                        std::cout << "The client disconnected" << std::endl;
                    }
                    else
                    {
                        //Display message
                        std::cout << "CLIENT:\n" << std::string(buf, 0, bytesRecv) << std::endl;
                        
                        //Generating json files
                        generateJson();

                        //Sending the message
                        sMessage_final = "Generating json files finished!";
                        send(clientSocket, sMessage_final.c_str(), sMessage_final.size(), 0);
                        std::cout << "Status: SENT\n";

                        message ++;
                        ros::shutdown();
                    }
                }
            }
        }
        
        void closingTheClientSocket()
        {
            //Close the socket
            std::cout << "Status: SHUTDOWN! " << std::endl;
            close(clientSocket); 
        }

        void robotDataUpdate(const nav_msgs::Odometry::ConstPtr& msg)
        {
            if(message == 2)
            {
                std::cout << "\nUpdating robot data..." << std::endl;
                // std::cout << "Press CTRL + C to finish updating!" << std::endl;
                json_data_2["Robot"]["Robot Name"] = "TurtleBot3";
                json_data_2["Robot"]["Position"]["X"] = msg -> pose.pose.position.x;
                json_data_2["Robot"]["Position"]["Y"] = msg -> pose.pose.position.y;
                json_data_2["Robot"]["Position"]["Z"] = msg -> pose.pose.position.z;
                json_data_2["Robot"]["Orientation"]["W"] = msg -> pose.pose.orientation.w;
                json_data_2["Robot"]["Orientation"]["X"] = msg -> pose.pose.orientation.x;
                json_data_2["Robot"]["Orientation"]["Y"] = msg -> pose.pose.orientation.y;
                json_data_2["Robot"]["Orientation"]["Z"] = msg -> pose.pose.orientation.z;
                std::cout << std::setw(4) << json_data_2 << std::endl;

                tcp_ip_client_sub.shutdown();
                rosfinished = true;
            }
        }

        void closestLandmark(const turtlebot3_msgs::LandmarkDistance::ConstPtr &msg)
        {
            if(message == 3)
            {
                std::cout << "\nUpdating closest landmark..." << std::endl;
                // std::cout << "Press CTRL + C to finish updating!" << std::endl;
                json_data_3["Closest landmark"]["Name"] = msg -> name;
                json_data_3["Closest landmark"]["Distance"] = msg -> distance;
                std::cout << std::setw(4) << json_data_3 << std::endl;

                rosfinished = true;
            }

            if(message == 4)
            {
                std::cout << "The current closest landmark is: " << msg -> name << std::endl;

                std::string string1 = msg -> name;
                std::string string2 = json_data_4["Goal landmark"]["LANDMARK NAME"].get<std::string>();

                if(string1.compare(string2) != 0)
                {
                    //Updatin json file
                    json_data_4["Closest landmark"]["LANDMARK NAME"] = msg -> name;

                    //Converting to string
                    sMessage_4 = json_data_4.dump(4);
                    std::cout << sMessage_4 << std::endl;

                    //Sending back to the client
                    send(clientSocket, sMessage_4.c_str(), sMessage_4.size(), 0);
                }

                //Sending the last message when the robot reached the position
                else
                {
                    //Updating json file
                    json_data_4["Closest landmark"]["LANDMARK NAME"] = msg -> name;

                    // Converting to string
                    sMessage_4 = json_data_4.dump(4);
                    std::cout << sMessage_4 << std::endl;
                    std::string finish_mess = "Robot reached the desired landmark!";
                    
                    //Sending back to the client
                    send(clientSocket, sMessage_4.c_str(), sMessage_4.size(), 0);
                    send(clientSocket, finish_mess.c_str(), finish_mess.size(), 0);

                    rosfinished = true;
                }
            }
        }
};

int main(int argc, char **argv)
{
    ros::init(argc, argv, "server_tcp_ip_node");

    TcpIpServer tcpIp;

    int step = 0;
    int returned_step = 0;

    if(step == 0)
    {
        returned_step = tcpIp.createSocket();
        if (returned_step != -1)
        {
            step++;
        }
    }
    if(step == 1)
    {
        returned_step = tcpIp.bindSocketAndIp();
        if (returned_step != -2)
        {
            step++;
        }
    }
    if(step == 2)
    {
        returned_step = tcpIp.listenToTheSocket();
        if (returned_step != -3)
        {
            step++;
        }
    }
    if(step == 3)
    {
        returned_step = tcpIp.acceptClientCall();
        if (returned_step != -4)
        {
            step++;
        }
    }
    if(step == 4)
    {
        tcpIp.communicate();
        step++;
    }
    if(step == 5)
    {
        tcpIp.closingTheClientSocket();
    }

    return 0;
}