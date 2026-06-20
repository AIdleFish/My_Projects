#pragma once
#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include "Common.h"
#include <unordered_map>// 包含 unordered_map 头文件，用于使用哈希表数据结构
// 用于UID找Socket和Socket找UID的双向映射，提供快速的查找和插入操作
class ConnectionManager {
private:
	// 使用两个 unordered_map 来实现用户 UID 和 Socket 之间的双向映射，时间复杂度为 O(1)
	std::unordered_map<int, SOCKET> uid_to_socket_;// 使用unordered_map存储用户 UID 和对应的Socket，提供快速的查找和插入操作
	std::unordered_map<SOCKET, int> socket_to_uid_;// 使用unordered_map存储 Socket 和对应的用户 UID，提供快速的查找和插入操作
	mutable std::mutex mtx_;// 定义一个互斥锁 mtx_，用于保护 uid_to_socket_ 和 socket_to_uid_ 这两个哈希表的访问，确保线程安全

public:
	void register_user(int uid, SOCKET sock);// 注册用户连接，将用户 UID 和 Socket 进行关联，存储在 uid_to_socket_ 和 socket_to_uid_ 中
	void unregister_socket(SOCKET sock);// 注销用户连接，根据 Socket 查找对应的用户 UID，并从 uid_to_socket_ 和 socket_to_uid_ 中删除相关记录
	SOCKET get_socket(int uid) const;// 根据用户 UID 获取对应的 Socket
	int get_uid(SOCKET sock) const;// 根据 Socket 获取对应的用户 UID
	bool is_connected(int uid) const;// 检查用户是否已连接
};

#endif
