#include "UserManager.h"
#include <wincrypt.h>
#include <iomanip>
#include <sstream>
#include <iostream>

#pragma comment(lib, "crypt32.lib")

std::string md5_hash(const std::string& input) {
	HCRYPTPROV prov = 0;
	HCRYPTHASH hash = 0;
	if (!CryptAcquireContext(&prov, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
		return input;
	}
	if (!CryptCreateHash(prov, CALG_MD5, 0, 0, &hash)) {
		CryptReleaseContext(prov, 0);
		return input;
	}
	if (!CryptHashData(hash, reinterpret_cast<const BYTE*>(input.c_str()),
		static_cast<DWORD>(input.size()), 0)) {
		CryptDestroyHash(hash);
		CryptReleaseContext(prov, 0);
		return input;
	}
	BYTE digest[16];
	DWORD len = 16;
	if (!CryptGetHashParam(hash, HP_HASHVAL, digest, &len, 0)) {
		CryptDestroyHash(hash);
		CryptReleaseContext(prov, 0);
		return input;
	}
	CryptDestroyHash(hash);
	CryptReleaseContext(prov, 0);

	std::ostringstream oss;
	for (DWORD i = 0; i < len; ++i) {
		oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(digest[i]);
	}
	return oss.str();
}

UserManager::UserManager(DBHelper& db) : db_(db) {}

bool UserManager::user_exists(int uid) {
	std::string sql= "SELECT id FROM user WHERE uid=" + std::to_string(uid) ;
	MYSQL_RES* res= db_.QuerySQL(sql.c_str());
	if (mysql_num_rows(res) > 0) return true;
	mysql_free_result(res);
	return false;
}

bool UserManager::username_exists(const std::string& username) {
	std::string sql = "SELECT id FROM user WHERE username=" + username;
	MYSQL_RES* res = db_.QuerySQL(sql.c_str());
	if (mysql_num_rows(res) > 0) return true;
	mysql_free_result(res);
	return false;
}

int UserManager::get_status(int uid) {
	std::string sql = "SELECT status FROM user WHERE uid=" + std::to_string(uid);
	MYSQL_RES* res = db_.QuerySQL(sql.c_str());
	if (!res) return -1;
	MYSQL_ROW row = mysql_fetch_row(res);
	int status = row ? atoi(row[0]) : -1;
	mysql_free_result(res);
	return status;
}

bool UserManager::set_status(int uid, int status) {
	std::string sql = "UPDATE user SET status=" + std::to_string(status)
		+ " WHERE uid=" + std::to_string(uid);
	return db_.ExecuteSQL(sql.c_str());
}

bool UserManager::add_user(const std::string& username, const std::string& password, int uid) {
	if (username_exists(username)) {
		std::cout << "用户名 " << username << " 已存在" << std::endl;
		return false;
	}
	if (user_exists(uid)) {
		std::cout << "UID " << uid <<" 已存在" << std::endl;
		return false;
	}
	std::string pwd = md5_hash(password);// 使用MD5算法加密密码为32位字符
	std::string sql = "INSERT INTO user(username, password, status, uid) VALUES('"
		+ db_.escape(username) + "','" + pwd + "',0," + std::to_string(uid) + ")";
	if (db_.ExecuteSQL(sql.c_str())) {
		std::cout << "用户添加成功" << std::endl;
		return true;
	}
	return false;
}

bool UserManager::update_user(int id, const std::string& username,
	const std::string& password, int uid, int status) {
	std::string pwd = md5_hash(password);
	std::string sql = "UPDATE user SET username='" + db_.escape(username)
		+ "',password='" + pwd + "',uid=" + std::to_string(uid)
		+ ",status=" + std::to_string(status) + " WHERE id=" + std::to_string(id);
	if (db_.ExecuteSQL(sql.c_str())) {
		std::cout << "用户更新成功" << std::endl;
		return true;
	}
	return false;
}

bool UserManager::delete_user(int id) {
	std::string sql = "DELETE FROM user WHERE id=" + std::to_string(id);
	if (db_.ExecuteSQL(sql.c_str())) {
		std::cout << "用户删除成功" << std::endl;
		return true;
	}
	return false;
}

// 获取所有用户信息保存在列表中
std::vector<UserInfo> UserManager::list_users() {
	std::vector<UserInfo> users;
	MYSQL_RES* res = db_.QuerySQL("SELECT id,uid,username,password,status FROM user");
	if (!res) return users;
	MYSQL_ROW row;
	while ((row = mysql_fetch_row(res))) {
		UserInfo u;
		u.id = atoi(row[0]);
		u.uid = atoi(row[1]);
		u.username = row[2] ? row[2] : "";
		u.password = row[3] ? row[3] : "";
		u.status = atoi(row[4]);
		users.push_back(u);
	}
	mysql_free_result(res);
	return users;
}

void UserManager::print_users() {
	auto users = list_users();// users本质是一个vector数组
	if (users.empty()) {
		std::cout << "暂无用户" << std::endl;
		return;
	}
	std::cout << "========== 用户列表 ==========" << std::endl;
	std::cout << std::left << std::setw(6) << "ID"
		<< std::setw(10) << "UID"
		<< std::setw(16) << "用户名"
		<< std::setw(8) << "状态" << std::endl;
	for (const auto& u : users) {
		std::cout << std::left << std::setw(6) << u.id
			<< std::setw(10) << u.uid
			<< std::setw(16) << u.username
			<< std::setw(8) << (u.status == 1 ? "在线" : "离线") << std::endl;
	}
	std::cout << "===============================" << std::endl;
}

void UserManager::run_menu() {
	while (true) {
		std::cout << "\n--- 用户账号管理 ---" << std::endl;
		std::cout << "1. 查看所有用户" << std::endl;
		std::cout << "2. 添加用户" << std::endl;
		std::cout << "3. 修改用户" << std::endl;
		std::cout << "4. 删除用户" << std::endl;
		std::cout << "输入 back 返回主菜单" << std::endl;
		std::cout << "请选择: ";
		std::string choice;
		std::cin >> choice;
		if (choice == "back") {
			system("clear");
			return;
		}

		if (choice == "1") {
			print_users();
		}
		else if (choice == "2") {
			std::string username, password;
			int uid;
			std::cout << "用户名: "; std::cin >> username;
			std::cout << "密码: "; std::cin >> password;
			std::cout << "UID: "; std::cin >> uid;
			add_user(username, password, uid);
		}
		else if (choice == "3") {
			print_users();
			int id, uid, status;
			std::string username, password;
			std::cout << "用户ID: "; std::cin >> id;
			std::cout << "新用户名: "; std::cin >> username;
			std::cout << "新密码: "; std::cin >> password;
			std::cout << "新UID: "; std::cin >> uid;
			std::cout << "状态(0离线/1在线): "; std::cin >> status;
			update_user(id, username, password, uid, status);
		}
		else if (choice == "4") {
			print_users();
			int id;
			std::cout << "要删除的用户ID: "; std::cin >> id;
			delete_user(id);
		}
		else {
			std::cout << "无效选择" << std::endl;
		}
	}
}
