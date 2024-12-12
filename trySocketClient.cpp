#include <stdlib.h>
#include <string>
#include <windows.h>
#include <iostream>
#pragma comment(lib, "wsock32.lib")
#include "nlohmann/json.hpp"


class TrafficLightInfoClient {

private:

	std::string serverIP;
	unsigned char serverPort;
	
public:

	TrafficLightInfoClient(std::string init_ip = "127.0.0.1", unsigned char init_port = 8848) {
	/*
	* @brief 类构造函数
	* 
	* @param init_ip:构造时要设置的ip,默认为127.0.0.1
	* @param init_port:构造时要设置的端口，默认为8848
	*
	*/

		serverIP = init_ip;
		serverPort = init_port;
	}

	std::string sendRequest(std::string message) {
		/*
		* 
		* @param 
		* 
		*/

		int clientSocket;
		struct sockaddr_in serverAddr;
		char buffer[1024];


		//创建套接字
		clientSocket = socket(AF_INET, SOCK_STREAM, 0);
		//判断套接字创建是否成功
		if (clientSocket < 0) {
			std::cerr<< "[ERROR]套接字创建失败！" << std::endl;
			return "";
		}

		//清空结构体
		memset(&serverAddr, 0, sizeof(serverAddr));
		//写入ip和端口
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(serverPort);
		serverAddr.sin_addr.s_addr = inet_addr(serverIP.c_str()); //注意这要转成c风格字符串

		//连接到服务器
		if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
			perror("Connection failed");
			closesocket(clientSocket);
			return "";
		}

		try {

			//将message序列化为json串
			auto message_json = nlohmann::json::parse(message);

			//判断action字段
			if (message_json["action"] == "write") {
				//执行相关写操作

			}
			else if (message_json["action"] == "read") {
				//执行相关读操作

			}


		}






	}

};