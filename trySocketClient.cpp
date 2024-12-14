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
		* �Է�����ִ��ͨ�Ų���
		* 
		* @param message:Ҫ���͵���Ϣ
		* @return ��Ӧ��Ϣ
		* 
		*/

		int clientSocket;
		struct sockaddr_in serverAddr;
		char buffer[1024];
		WSADATA wsaData;

		// ��ʼ�� Winsock
		int wsResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (wsResult != 0) {
			std::cerr << u8"[ERROR] Winsock ��ʼ��ʧ�ܣ��������: " << wsResult << std::endl;
			return "";
		}

		//�����׽���
		clientSocket = socket(AF_INET, SOCK_STREAM, 0);
		//�ж��׽��ִ����Ƿ�ɹ�
		if (clientSocket == INVALID_SOCKET) {
			std::cerr << u8"[ERROR]�׽��ִ���ʧ�ܣ��������: " << WSAGetLastError() << std::endl;
			return "";
		}

		//���ó�ʱʱ�䣨ms��
		DWORD timeout = 5000;

		if (setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) < 0) {
			std::cerr << u8"[ERROR]���ý��ճ�ʱʧ��: " << strerror(errno) << std::endl;
			closesocket(clientSocket);
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
			perror(u8"[ERROR]���ӷ�����ʧ�ܣ�");
			closesocket(clientSocket);
			return "";
		}

		// ������Ϣ(������Բ��ô�����룬���ᰴ��ԭ�б���д��)
		if (send(clientSocket, message.c_str(), message.size(), 0) < 0) {
			std::cerr << u8"[ERROR]��������ʧ��: " << strerror(errno) << std::endl;
			closesocket(clientSocket);
			return "";
		}

		// ���շ�������Ӧ
		int bytes_received = recv(clientSocket, buffer, sizeof(buffer), 0);
		if (bytes_received < 0) {
			std::cerr << u8"[ERROR]��������ʧ��: " << strerror(errno) << std::endl;
			closesocket(clientSocket);
			return "";
		}

		// ����Ӧת��Ϊ�ַ���������
		std::string response(buffer, bytes_received);

		// �ر� socket
		closesocket(clientSocket);

		return response;


	}

	std::string write(nlohmann::json write_data) {
		/*
		* 
		* д�����ݵ������
		* @param write_data Ҫд�������(json���������neckName�ֶ�)
		* @return ����˵���Ӧ
		* 
		*/
	
		//�ж�write_data���Ƿ���neckName�ֶ�
		if (write_data["neckName"] == NULL) {
			std::cout << u8"[ERROR]д�����ݱ������ 'neckName' �ֶΣ�" << std::endl;
			return "";
		}

		

		//��дmessage(��json����д)

		nlohmann::json message;
		message["action"] = "write";
		message["neckName"] = write_data["neckName"];
		message["data"] = write_data;
		
		std::string message_str = message.dump();
		//���Ͳ�������Ӧ
		std::string response = sendRequest(message_str);
		
		std::cout << u8"��ȡ������Ӧ��" << response << std::endl;

		return response;
	}
};

int main() {

	system("CHCP 65001");
	nlohmann::json write_data = {
		{"neckName", u8"��У·��"},
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
