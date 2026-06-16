#pragma once
#ifndef DB_HELPER_H
#define DB_HELPER_H

#pragma execution_character_set("utf-8")

#include"config.h"
#include<iostream>
#include<mysql.h>

class DBHelper {
private:
	MYSQL mysql;
	bool connected;//Mysql连接状态，False未连接，Ture连接
public:
	DBHelper();//构造函数
	~DBHelper();//析构函数，用于断开Mysql连接
	bool connectDB();
	bool executeSQL(const char* sql);//执行SQL语句（增删改)
	MYSQL_RES* querySQL(const char* sql);//用于执行SQL语句（查）
	void closeDB();//用于关闭Mysql,断开连接
	bool isconnected() {
		return connected;//返回连接结果
	}
};

#endif // !DB_HELPER_H
