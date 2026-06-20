#pragma once
#ifndef DBHELPER_H
#define DBHELPER_H

#include "Common.h"
#include "ClientConfig.h"
#include <string>

class DBHelper {
private:
	MYSQL mysql;
	bool connected;
public:
	DBHelper();
	~DBHelper();
	bool ExecuteSQL(const char* query);
	MYSQL_RES* QuerySQL(const char* query);
	bool connectDB();
	void closeDB();
	bool isConnected() const { return connected; }
	std::string escape(const std::string& str);

};

#endif // !DBHELPER_H
