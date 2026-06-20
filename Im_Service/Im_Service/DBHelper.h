#pragma once
#ifndef DBHELPER_H
#define DBHELPER_H

#include "Common.h"
#include "Config.h"
#include <string>

class DBHelper {
private:
	MYSQL mysql;
	bool connected;
public:
	DBHelper();
	~DBHelper();
	bool ExecuteSQL(const char* query);// 执行SQL语句，返回是否成功
	MYSQL_RES* QuerySQL(const char* query);// 执行查询SQL语句，返回结果集指针，如果失败返回NULL
	bool connectDB();// 连接数据库，返回是否成功
	void closeDB();// 关闭数据库连接
	bool isConnected() const { return connected; }// 检查数据库连接状态，返回是否已连接
	std::string escape(const std::string& str);// 转义字符串，防止SQL注入攻击
};

#endif
