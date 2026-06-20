#pragma once
#ifndef USERMANAGER_H
#define USERMANAGER_H

#include "DBHelper.h"
#include <string>
#include <vector>

struct UserInfo {// 用户信息字段结构体
	int id;
	int uid;
	std::string username;
	std::string password;
	int status;
};

class UserManager {
private:
	DBHelper& db_;// 直接使用数据库连接实体

public:
	explicit UserManager(DBHelper& db);
	bool user_exists(int uid);// 用户是否已注册
	bool username_exists(const std::string& username);// 用户名是否存在
	int get_status(int uid);// 获取用户在线状态
	bool set_status(int uid, int status);// 设置用户在线状态
	bool add_user(const std::string& username, const std::string& password, int uid);// 添加用户
	bool update_user(int id, const std::string& username, const std::string& password, int uid, int status);// 更新用户
	bool delete_user(int id);// 删除用户
	std::vector<UserInfo> list_users();// 用户信息列表
	void print_users();// 输出所有用户
	void run_menu();// 运行菜单
};

std::string md5_hash(const std::string& input);// 使用MD5加密算法加密密码

#endif
