#pragma once
#ifndef TCPSERVER_H
#define TCPSERVER_H
#include "Common.h"
#include "ThreadPool.h"
#include "MessageService.h"
#include "ConnectionManager.h"

class TcpServer {
private:
	SOCKET listen_socket_;
	bool running_;
	MessageService& msg_service_;
	ConnectionManager& conn_mgr_;

public:
	TcpServer(MessageService& msg_service, ConnectionManager& conn_mgr);
	~TcpServer();
	bool init(int port);// 服务器初始化
	void start_listening(ThreadPool& thread_pool);// 开始监听客户端
	void close();// 关闭服务器
	bool is_running() const { return running_; }// 是否在运行

private:
	void handle_client(SOCKET client_socket);// 处理客户端
};
#endif
