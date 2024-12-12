#include <iostream>
#include <string>
#include <cstring>
#include <chrono>
#include <winsock2.h>     // Windows 套接字 API
#include <ws2tcpip.h>     // 用于 inet_pton() 和 sockaddr_in

#pragma comment(lib, "ws2_32.lib")  // 链接 Winsock 库

void udp_client(const std::string& server_ip, int server_port, const std::string& message) {
    // 初始化 Winsock
    WSADATA wsaData;
    int wsaStartupResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsaStartupResult != 0) {
        std::cerr << "Winsock 初始化失败！" << std::endl;
        return;
    }

    // 创建套接字
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "无法创建套接字!" << std::endl;
        WSACleanup();
        return;
    }

    // 设置服务器地址
    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port); // 转换为网络字节序
    inet_pton(AF_INET, server_ip.c_str(), &server_addr.sin_addr);

    // 记录发送时间
    auto send_time = std::chrono::high_resolution_clock::now();

    // 发送消息
    size_t sent_len = sendto(sock, message.c_str(), message.length(), 0,
        (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (sent_len == SOCKET_ERROR) {
        std::cerr << "发送失败!" << std::endl;
        closesocket(sock);
        WSACleanup();
        return;
    }

    // 接收响应
    char buffer[4096];
    int server_addr_len = sizeof(server_addr);
    size_t recv_len = recvfrom(sock, buffer, sizeof(buffer), 0,
        (struct sockaddr*)&server_addr, &server_addr_len);
    if (recv_len == SOCKET_ERROR) {
        std::cerr << "接收失败!" << std::endl;
        closesocket(sock);
        WSACleanup();
        return;
    }

    // 记录接收时间
    auto receive_time = std::chrono::high_resolution_clock::now();

    // 计算往返延迟 (RTT)
    std::chrono::duration<double> round_trip_time = receive_time - send_time;
    double rtt_ms = round_trip_time.count() * 1000; // 转换为毫秒

    // 输出响应和 RTT
    std::cout << "收到来自服务器的响应: " << std::string(buffer, recv_len) << std::endl;
    std::cout << "往返延迟（RTT）：" << rtt_ms << " 毫秒" << std::endl;

    // 关闭套接字
    closesocket(sock);
    WSACleanup();  // 清理 Winsock
}

int main() {
    int i = 0;

    // 发送消息给服务器，执行周期性任务
    while (i<10) {
        
        std::string message = "{\"string\": \"helloworld!\"}";
        
        std::cout <<"向服务器发送：" << message << std::endl;
        
        udp_client("127.0.0.1", 52362, message);

        i++;

        // 每 0.5 秒发送一次
        Sleep(500);  // Sleep 以毫秒为单位，Windows 中使用 Sleep 函数
    }

    return 0;
}
