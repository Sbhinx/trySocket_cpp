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
	* @brief �๹�캯��
	* 
	* @param init_ip:����ʱҪ���õ�ip,Ĭ��Ϊ127.0.0.1
	* @param init_port:����ʱҪ���õĶ˿ڣ�Ĭ��Ϊ8848
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


		//�����׽���
		clientSocket = socket(AF_INET, SOCK_STREAM, 0);
		//�ж��׽��ִ����Ƿ�ɹ�
		if (clientSocket < 0) {
			std::cerr<< "[ERROR]�׽��ִ���ʧ�ܣ�" << std::endl;
			return "";
		}

		//��սṹ��
		memset(&serverAddr, 0, sizeof(serverAddr));
		//д��ip�Ͷ˿�
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(serverPort);
		serverAddr.sin_addr.s_addr = inet_addr(serverIP.c_str()); //ע����Ҫת��c����ַ���

		//���ӵ�������
		if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
			perror("Connection failed");
			closesocket(clientSocket);
			return "";
		}

		try {

			//��message���л�Ϊjson��
			auto message_json = nlohmann::json::parse(message);

			//�ж�action�ֶ�
			if (message_json["action"] == "write") {
				//ִ�����д����

			}
			else if (message_json["action"] == "read") {
				//ִ����ض�����

			}


		}






	}

};