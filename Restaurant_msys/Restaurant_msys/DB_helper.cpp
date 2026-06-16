#include"DB_helper.h"
#include<iostream>
using namespace std;
//建立数据库初始化连接
DBHelper::DBHelper() : connected(false) {
    MYSQL* ret = mysql_init(&mysql);
    if (ret == NULL) {
        throw std::runtime_error(std::string("数据库初始化失败: ") + mysql_error(&mysql));
    }
	mysql_set_character_set(&mysql, "utf8");
}
//与数据库断开连接,析构函数,释放资源,防止内存泄漏
DBHelper::~DBHelper() {
	closeDB();
}
//连接数据库
bool DBHelper::connectDB() {
	if (!mysql_real_connect(&mysql, DB_HOST, DB_USER, DB_PASS, DB_NAME, DB_PORT, NULL, 0)) {
		cout << "数据库连接失败! " << mysql_error(&mysql) << endl;
		return false;
	}
	connected = true;
	cout << "数据库连接成功! " << endl;
	return true;
}
//执行SQL语句
bool DBHelper::executeSQL(const char* sql) {
	if (!connected) return false;
	return mysql_query(&mysql, sql) == 0;//mysql_query函数返回0表示执行成功
}
//执行SQL查询语句，返回的是查询结果
MYSQL_RES* DBHelper::querySQL(const char* sql) {
	if (!connected) return NULL;
	if (mysql_query(&mysql, sql) != 0) return NULL;//mysql_query函数返回非0表示执行失败
	return mysql_store_result(&mysql);
}
//关闭连接
void DBHelper::closeDB() {
	if (connected) {
		mysql_close(&mysql);
		connected = false;
	}
}
