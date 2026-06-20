#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "TcpClient.h"
#include <iostream>

TcpClient::TcpClient()
	: sock_(INVALID_SOCKET), connected_(false), running_(false) {}

TcpClient::~TcpClient() {
	disconnect();
}

bool TcpClient::connect_to(const char* host, int port) {
	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
		std::cerr << "WSAStartup 失败" << std::endl;
		return false;
	}

	sock_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock_ == INVALID_SOCKET) {
		std::cerr << "创建 Socket 失败: " << WSAGetLastError() << std::endl;
		return false;
	}

	sockaddr_in server_addr = { 0 };
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(static_cast<u_short>(port));
	if (inet_pton(AF_INET, host, &server_addr.sin_addr) <= 0) {
		server_addr.sin_addr.s_addr = inet_addr(host);
	}

	if (::connect(sock_, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr)) == SOCKET_ERROR) {
		std::cerr << "连接服务器失败: " << WSAGetLastError() << std::endl;
		closesocket(sock_);
		sock_ = INVALID_SOCKET;
		return false;
	}

	connected_ = true;
	std::cout << "已连接到 " << host << ":" << port << std::endl;
	return true;
}

bool TcpClient::send_message(const std::string& data) {
	if (!connected_) return false;
	return send_packet(sock_, data);
}

void TcpClient::recv_loop(std::function<void(const std::string&)> on_message) {
	std::string packet;
	while (running_) {
		if (!recv_packet(sock_, packet)) {
			if (running_) {
				std::lock_guard<std::mutex> lock(print_mtx_);
				std::cout << "\n[系统] 与服务器断开连接" << std::endl;
			}
			connected_ = false;
			running_ = false;
			break;
		}
		if (on_message) {
			on_message(packet);
		}
	}
}

void TcpClient::start_receive(std::function<void(const std::string&)> on_message) {
	running_ = true;
	recv_thread_ = std::thread(&TcpClient::recv_loop, this, on_message);
}

void TcpClient::disconnect() {
	running_ = false;
	connected_ = false;
	if (sock_ != INVALID_SOCKET) {
		shutdown(sock_, SD_BOTH);
		closesocket(sock_);
		sock_ = INVALID_SOCKET;
	}
	if (recv_thread_.joinable()) {
		recv_thread_.join();
	}
	WSACleanup();
}

bool TcpClient::is_connected() const {
	return connected_;
}
