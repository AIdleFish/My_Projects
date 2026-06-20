#pragma once
#ifndef LOGMANAGER_H
#define LOGMANAGER_H

#include "Common.h"
#include <chrono>
#include <iomanip>// 包含 <iomanip> 头文件，用于处理格式化输出
#include <sstream>// 包含 <sstream> 头文件，用于使用字符串流进行字符串操作
#include <vector>

struct LogEntry {
	std::string time;// 日志时间，格式为 "YYYY-MM-DD HH:MM:SS"
	std::string raw_message;// 原始消息内容
	std::string sender_uid;// 发送者 UID
	std::string receiver_uid;// 接收者 UID
	std::string content;// 消息内容
};

class LogManager {
private:
	std::vector<LogEntry> logs_;
	mutable std::mutex mtx_;

public:
	void add(const std::string& raw, const std::string& sender,
		const std::string& receiver, const std::string& content);// 添加日志记录，参数包括原始消息内容、发送者 UID、接收者 UID 和消息内容
	void print_all() const;// 打印所有日志记录，输出格式为 "[时间] 发送者UID -> 接收者UID : 消息内容"
	void clear();// 清除所有日志记录
	size_t count() const;// 获取日志记录的数量
};

#endif
