#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "TcpServer.h"
#include "Protocol.h"
#include "UserManager.h"
#include <iostream>

TcpServer::TcpServer(MessageService& msg_service, ConnectionManager& conn_mgr)
	: listen_socket_(INVALID_SOCKET), running_(false),
	msg_service_(msg_service), conn_mgr_(conn_mgr) {}

TcpServer::~TcpServer() {
	close();
}

bool TcpServer::init(int port) {
	WSADATA wsadata;// 获取本地网络权限
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
		std::cerr << "WSAStartup失败" << std::endl;
		return false;
	}
	listen_socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);// 绑定监听器属性 使用IPV4(AF_INET) 使用TCP协议
	if (listen_socket_ == INVALID_SOCKET) {
		std::cerr << "Socket创建失败 " << WSAGetLastError() << std::endl;
		return false;
	}

	int opt = 1;
	setsockopt(listen_socket_, SOL_SOCKET, SO_REUSEADDR,
		reinterpret_cast<const char*>(&opt), sizeof(opt));
	//C++ 强制类型转换：把 int 变量地址转成 API 需要的 const char* 通用字节指针，适配系统 C 接口

	// 设置服务器属性，IP地址，端口等信息
	sockaddr_in server_addr = { 0 };
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(static_cast<u_short>(port));
	if (::bind(listen_socket_, reinterpret_cast<sockaddr*>(&server_addr),
		sizeof(server_addr)) == SOCKET_ERROR) {
		std::cerr << "Socket绑定失败 " << WSAGetLastError() << std::endl;
		closesocket(listen_socket_);
		return false;
	}
	if (::listen(listen_socket_, SOMAXCONN) == SOCKET_ERROR) {
		std::cerr << "Socket监听失败 " << WSAGetLastError() << std::endl;
		closesocket(listen_socket_);
		return false;
	}
	running_ = true;
	std::cout << "服务器在端口 " << port << " 上启动" << std::endl;
	return true;
}

void TcpServer::start_listening(ThreadPool& thread_pool) {
	while (running_) {
		sockaddr_in client_addr = { 0 };
		int client_addr_len = sizeof(client_addr);
		SOCKET client_socket = accept(listen_socket_,
			reinterpret_cast<sockaddr*>(&client_addr), &client_addr_len);
		if (client_socket == INVALID_SOCKET) {
			if (running_) {
				std::cerr << "接受连接失败 " << WSAGetLastError() << std::endl;
			}
			continue;
		}
		std::cout << "客户端连接: " << inet_ntoa(client_addr.sin_addr)
			<< ":" << ntohs(client_addr.sin_port) << std::endl;
		thread_pool.enqueue([this, client_socket]() {
			handle_client(client_socket);
			// 服务完成后，用户离线
			int uid = conn_mgr_.get_uid(client_socket);
			if (uid > 0) {
				msg_service_.on_user_offline(uid);
				std::cout << "客户" << uid << "下线" << std::endl;
			}
			conn_mgr_.unregister_socket(client_socket);
			closesocket(client_socket);
		});
	}
}

void TcpServer::close() {
	if (!running_) return;
	running_ = false;
	if (listen_socket_ != INVALID_SOCKET) {
		closesocket(listen_socket_);
		listen_socket_ = INVALID_SOCKET;
	}
	std::cout << "服务器已停止" << std::endl;
}

void TcpServer::handle_client(SOCKET client_socket) {
	std::string buffer;
	int uid = conn_mgr_.get_uid(client_socket);
	while (true) {
		if (!recv_packet(client_socket, buffer)) {
			break;
		}
		msg_service_.handle_message(buffer, client_socket);
	}
}
