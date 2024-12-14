#include <stdlib.h>
#include <string>
#include <windows.h>
#include <iostream>
#pragma comment(lib, "wsock32.lib")
#include "nlohmann/json.hpp"
#include <time.h>
#pragma comment(lib, "ws2_32.lib")


class TrafficLightInfoClient {

private:

	std::string serverIP;
	int serverPort;
	
public:

	TrafficLightInfoClient(std::string init_ip = "127.0.0.1", int init_port = 8848) {
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
		* 对服务器执行通信操作
		* 
		* @param message:要发送的消息
		* @return 响应信息
		* 
		*/

		int clientSocket;
		struct sockaddr_in serverAddr;
		char buffer[1024];
		WSADATA wsaData;

		// 初始化 Winsock
		int wsResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (wsResult != 0) {
			std::cerr << u8"[ERROR] Winsock 初始化失败！错误代码: " << wsResult << std::endl;
			return "";
		}

		//创建套接字
		clientSocket = socket(AF_INET, SOCK_STREAM, 0);
		//判断套接字创建是否成功
		if (clientSocket == INVALID_SOCKET) {
			std::cerr << u8"[ERROR]套接字创建失败！错误代码: " << WSAGetLastError() << std::endl;
			return "";
		}

		//设置超时时间（ms）
		DWORD timeout = 5000;

		if (setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) < 0) {
			std::cerr << u8"[ERROR]设置接收超时失败: " << strerror(errno) << std::endl;
			closesocket(clientSocket);
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
			perror(u8"[ERROR]连接服务器失败！");
			closesocket(clientSocket);
			return "";
		}

		// 发送消息(这里可以不用处理编码，他会按照原有编码写入)
		if (send(clientSocket, message.c_str(), message.size(), 0) < 0) {
			std::cerr << u8"[ERROR]发送数据失败: " << strerror(errno) << std::endl;
			closesocket(clientSocket);
			return "";
		}

		// 接收服务器响应
		int bytes_received = recv(clientSocket, buffer, sizeof(buffer), 0);
		if (bytes_received < 0) {
			std::cerr << u8"[ERROR]接收数据失败: " << strerror(errno) << std::endl;
			closesocket(clientSocket);
			return "";
		}

		// 将响应转换为字符串并返回
		std::string response(buffer, bytes_received);

		// 关闭 socket
		closesocket(clientSocket);

		return response;


	}

	std::string write(nlohmann::json write_data) {
		/*
		* 
		* 写入数据到服务端
		* @param write_data 要写入的数据(json对象，需包含neckName字段)
		* @return 服务端的响应
		* 
		*/
	
		//判断write_data中是否有neckName字段
		if (write_data["neckName"] == NULL) {
			std::cout << u8"[ERROR]写入数据必须包含 'neckName' 字段！" << std::endl;
			return "";
		}

		

		//编写message(用json对象写)

		nlohmann::json message;
		message["action"] = "write";
		message["neckName"] = write_data["neckName"];
		message["data"] = write_data;
		
		std::string message_str = message.dump();
		//发送并接受响应
		std::string response = sendRequest(message_str);
		
		std::cout << u8"读取操作响应：" << response << std::endl;

		return response;
	}
};

int main() {

	system("CHCP 65001");
	nlohmann::json write_data = {
		{"neckName", u8"党校路口"},
		{"redLampRule" , 10},
		{"yellowLampRule" , 3},
		{"greenLampRule" , 50},
		{"lampColorSample" , 1},
		{"lampCountdownSample" , 10},
		{"datetimeSample" , "2023-06-06 14:30:00"}
	};


	TrafficLightInfoClient client("192.168.8.25", 8848);

	client.write(write_data);

	return 0;

}
