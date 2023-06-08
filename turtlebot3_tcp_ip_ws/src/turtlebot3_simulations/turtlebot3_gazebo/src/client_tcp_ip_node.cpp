#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>

#include "nlohmann/json.hpp"
#include <fstream>

#include <cstdlib>
#include <unistd.h>

const int PORT = 54000;

class TcpIpClient
{
    private:
        int sock;

    public:
        int createSocket()
        {
            // Create a socket

            sock = socket(AF_INET, SOCK_STREAM, 0);

            if (sock == -1)
            {
                return -1; //failed
            }
            else
            {
                return 0;
            }
        }

        int connectToTheServer()
        {
            //Create a hint structure for the server we're connecting with

            std::string ipAddress = "127.0.0.1";

            struct sockaddr_in hint;
            hint.sin_family = AF_INET;
            hint.sin_port = htons(PORT);
            inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

            std::cout << "Connecting to the server..." << std::endl;

            //Connect to the server on the socket

            int connectRes = connect(sock, (sockaddr*)&hint, sizeof(hint));

            if (connectRes == -1)
            {
                std::cerr << "Connection failed"<< std::endl;
                return 1;
            }
            else
            {
                std::cout << "Connected to the server" << std::endl;
                return 0;
            }
        }

        void communicate()
        {
            int message = 1;
            char buf[4096];
            char bufFinal[4096];
            int bytesReceived;
            bool flag = true;
            std::ifstream file("robot.json");
            std::ifstream file2("closest_landmark.json");
            std::ifstream file3("goal_landmark.json");
            nlohmann::json json_data;
            nlohmann::json json_data_2;
            std::string cMessages;
            std::string userInput;

            while(flag)
            {
                if (message == 1)
                {
                    std::cout << "\nMESSAGE 1\n";
                    memset(buf, 0, 4096);
                    bytesReceived = recv(sock, buf, 4096, 0);
                    if (bytesReceived == -1)
                    {
                        std::cout << "There was an error on the first message\r\n";
                    }
                    else
                    {
                        //Display response
                        std::cout << "SERVER:\n" << std::string(buf, bytesReceived) << "\r\n";
                        message ++;
                    }
                }

                if (message == 2)
                {
                    json_data = nlohmann::json::parse(file);
                    cMessages = json_data.dump(4);
                    std::cout << "\nMESSAGE 2\n";
                    send(sock, cMessages.c_str(), cMessages.size(), 0);
                    std::cout << "Status: SENT\n";
                    memset(buf, 0, 4096);
                    bytesReceived = recv(sock, buf, 4096, 0);
                    if (bytesReceived == -1)
                    {
                        std::cout << "There was an error on the second message\r\n";
                    }
                    else
                    {
                        //Display response
                        std::cout << "SERVER:\n" << std::string(buf, bytesReceived) << "\r\n";
                        message ++;
                    }
                }

                if (message == 3)
                {
                    json_data = nlohmann::json::parse(file2);
                    cMessages = json_data.dump(4);
                    std::cout << "\nMESSAGE 3\n";
                    send(sock, cMessages.c_str(), cMessages.size(), 0);
                    std::cout << "Status: SENT\n";
                    memset(buf, 0, 4096);
                    bytesReceived = recv(sock, buf, 4096, 0);
                    if (bytesReceived == -1)
                    {
                        std::cout << "There was an error on the third message\r\n";
                    }
                    else
                    {
                        //Display response
                        std::cout << "SERVER:\n" << std::string(buf, bytesReceived) << "\r\n";
                        message ++;
                    }
                }

                if (message == 4)
                {
                    json_data = nlohmann::json::parse(file3);
                    std::cout << "\nMESSAGE 4\n";
                    std::cout << "Go to the desired landmark Cylinder_(1-9)." << std::endl;
                    std::cout << "> ";
                    std::getline(std::cin, userInput);
                    json_data["Goal landmark"]["LANDMARK NAME"] = userInput;
                    cMessages = json_data.dump(4);

                    send(sock, cMessages.c_str(), cMessages.size(), 0);
                    std::cout << "Status: SENT\n";

                    while(bytesReceived <= 500) //141
                    {
                        memset(buf, 0, 4096);
                        bytesReceived = recv(sock, buf, 4096, 0);

                        if (bytesReceived == -1)
                        {
                            std::cout << "There was an error on the fourth message\r\n";
                        }
                        else
                        {
                            //Signal that the last message is sent
                            if(bytesReceived == 35 || bytesReceived == 176)
                            {
                                std::cout << "SERVER:\n" << std::string(buf, bytesReceived) << "\r\n";
                                // memset(buf, 0, 4096);

                                // std::cout << bytesReceived << std::endl;
                                message ++;
                                goto label2;
                            }

                            //Display response
                            std::cout << "SERVER:\n" << std::string(buf, bytesReceived) << "\r\n";
                        }
                        // std::cout << bytesReceived << std::endl;
                    }
                }

                if (message == 5)
                {
                    label2:
                    cMessages = "Generate a json files with the entire conversation.";
                    std::cout << "\nMESSAGE 5\n";
                    send(sock, cMessages.c_str(), cMessages.size(), 0);
                    std::cout << "Status: SENT\n";
                    memset(bufFinal, 0, 4096);
                    bytesReceived = recv(sock, bufFinal, 4096, 0);
                    if (bytesReceived == -1)
                    {
                        std::cout << "There was an error on the fifth message\r\n";
                    }
                    else
                    {
                        //Display response
                        std::cout << "SERVER:\n" << std::string(bufFinal, bytesReceived) << "\r\n";
                        // std::ifstream file7("goal_landmark_updated.json");
                        // std::cout << file7.rdbuf() << std::endl;
                        // file7.open("goal_landmark_updated.json");
                        
                        label:
                        //Checking if the last generated json file exists
                        if(std::ifstream ("goal_landmark_updated.json"))
                        {
                            std::cout << "File exists" << std::endl;

                            //Waiting for data to be written in the json files for 1 second
                            sleep(1);

                            std::ifstream file4("client_data_updated.json");
                            json_data = nlohmann::json::parse(file4);
                            std::cout << std::setw(4) << json_data << std::endl;        

                            std::ifstream file5("robot_updated.json");
                            json_data = nlohmann::json::parse(file5);
                            std::cout << std::setw(4) << json_data << std::endl;

                            std::ifstream file6("closest_landmark_updated.json");
                            json_data = nlohmann::json::parse(file6);
                            std::cout << std::setw(4) << json_data << std::endl;

                            std::ifstream file7("goal_landmark_updated.json");
                            json_data = nlohmann::json::parse(file7);
                            std::cout << std::setw(4) << json_data << std::endl;
                            
                            message ++;
                            flag = false;
                        }
                        else
                        {
                            std::cout << "File does not exist " << std::endl;
                            goto label;
                        }
                    }
                }
            }
            
        }

        void closingTheClientSocket()
        {
            //Close the socket
            std::cout << "Status: SHUTDOWN! " << std::endl;
            close(sock);
        }
};

int main(int argc, char **argv)
{
    TcpIpClient tcpIp;





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
        returned_step = tcpIp.connectToTheServer();
        if (returned_step != -2)
        {
            step++;
        }
    }
    if(step == 2)
    {
        tcpIp.communicate();
        
        step++;
    }
    if(step == 3)
    {
        tcpIp.closingTheClientSocket();

        step++;
    }

    return 0;
}
