#pragma once
#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include "Common.h"
#include "Protocol.h"
#include <functional>

class TcpClient {
private:
	SOCKET sock_;
	std::atomic<bool> connected_;
	std::atomic<bool> running_;
	std::thread recv_thread_;
	std::mutex print_mtx_;

	void recv_loop(std::function<void(const std::string&)> on_message);

public:
	TcpClient();
	~TcpClient();

	bool connect_to(const char* host, int port);
	bool send_message(const std::string& data);
	void start_receive(std::function<void(const std::string&)> on_message);
	void disconnect();
	bool is_connected() const;
};

#endif
