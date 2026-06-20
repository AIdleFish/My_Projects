#pragma once
#ifndef MESSAGESERVICE_H
#define MESSAGESERVICE_H

#include "DBHelper.h"
#include "ConnectionManager.h"
#include "LogManager.h"
#include "Protocol.h"

// 存储解析后的消息结构体
struct ParsedMessage {
	int sender_uid;
	int receiver_uid;
	std::string content;
	bool valid;// 解析是否成功
};

class MessageService {
private:
	DBHelper& db_;// 直接使用已连接的数据库实体
	ConnectionManager& conn_mgr_;// 连接器对象
	LogManager& log_mgr_;// 日志对象
	std::mutex cout_mtx_;

	bool save_offline_msg(int sender_uid, int receiver_uid, const std::string& content);// 保存离线消息到数据库
	void deliver_offline_messages(int uid, SOCKET sock);// 用户上线递送离线消息
	std::string build_message(int sender_uid, int receiver_uid, const std::string& content);// 将数据打包成协议格式

public:
	MessageService(DBHelper& db, ConnectionManager& conn_mgr, LogManager& log_mgr);
	static bool parse_message(const std::string& raw, ParsedMessage& out);// 将原始消息解析到out中
	void handle_message(const std::string& raw, SOCKET client_socket);// 处理消息
	void on_user_online(int uid, SOCKET sock);// 用户在线直接转发
	void on_user_offline(int uid);// 用户离线，保存离线消息
};

#endif
