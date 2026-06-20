#pragma once
#ifndef ONLINE_H
#define ONLINE_H
#include"DBHelper.h"
#include<string>

class OnLine {
private:
	DBHelper& db_;
public:
	OnLine(DBHelper& db);
	bool Register(const std::string& username, const std::string& password);
	std::string get_uid(const std::string& username);
};
std::string md5_hash(const std::string& input);
#endif // !LOG_IN_H
