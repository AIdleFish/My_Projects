#include "DBHelper.h"
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>
using namespace std;

DBHelper::DBHelper() : connected(false) {
	MYSQL* ret = mysql_init(&mysql);// 初始化MYSQL结构体，返回指向MYSQL结构体的指针，如果失败返回NULL
	if (!ret) {
		std::string msg = "MySQL初始化失败: ";
		msg += mysql_error(&mysql);
		throw std::runtime_error(msg);
	}
	mysql_set_character_set(&mysql, "utf8mb4");
}

DBHelper::~DBHelper() {
	if (connected) {
		closeDB();
	}
}

bool DBHelper::connectDB() {
	if (connected) {
		cout << "数据库已连接，无需重复连接" << endl;
		return true; 
	}
	if (!mysql_real_connect(&mysql, DB_HOST, DB_USER, DB_PASS, DB_NAME, DB_PORT, NULL, 0)) {
		cerr << "连接数据库失败: " << mysql_error(&mysql) << endl;
		return false;
	}
	connected = true;
	return true;
}

void DBHelper::closeDB() {
	if (connected) {
		mysql_close(&mysql);
		connected = false;
	}
}

bool DBHelper::ExecuteSQL(const char* query) {
	if (!connected) {
		cerr << "数据库未连接，无法执行SQL" << endl;
		return false;
	}
	if (mysql_query(&mysql, query) != 0) {
		cerr << "执行SQL失败: " << mysql_error(&mysql) << endl;
		return false;
	}
	return true;
}

MYSQL_RES* DBHelper::QuerySQL(const char* query) {
	if (!connected) {
		cerr << "数据库未连接, 无法执行查询" << endl;
		return NULL;
	}
	if (mysql_query(&mysql, query) != 0) {
		cerr << "执行查询失败: " << mysql_error(&mysql) << endl;
		return NULL;
	}
	MYSQL_RES* result = mysql_store_result(&mysql);
	if (!result) {
		cerr << "获取查询结果失败: " << mysql_error(&mysql) << endl;
		return NULL;
	}
	return result;
}

//防止SQL注入攻击，转义字符串中的特殊字符，如单引号、双引号、反斜杠等，使其在SQL语句中被正确解析，而不是被当作SQL语法的一部分，从而提高数据库的安全性
std::string DBHelper::escape(const std::string& str) {
	if (!connected) return str;// 如果未连接数据库，直接返回原字符串
	std::vector<char> buf(str.size() * 2 + 1);// 创建一个足够大的缓冲区，长度为原字符串的两倍加一，以存储转义后的字符串
	unsigned long len = mysql_real_escape_string(&mysql, buf.data(), str.c_str(),
		static_cast<unsigned long>(str.size()));// 调用 mysql_real_escape_string 函数进行字符串转义，传入 MYSQL 连接对象、缓冲区指针、原字符串指针和原字符串长度，返回转义后的字符串长度
	return std::string(buf.data(), len);// 将转义后的字符串构造为 std::string 对象并返回，使用缓冲区指针和转义后的字符串长度来创建 std::string 对象
}