#include "ConnectionManager.h"
// 上线的用户存放对应的UID和Socket，便于查找和操作
void ConnectionManager::register_user(int uid, SOCKET sock) {
	std::lock_guard<std::mutex> lock(mtx_);
	uid_to_socket_[uid] = sock;// 根据UID存Socket
	socket_to_uid_[sock] = uid;// 根据Socket存UID
}
// 离线的用户根据Socket注销，删除对应的UID和Socket记录
void ConnectionManager::unregister_socket(SOCKET sock) {
	std::lock_guard<std::mutex> lock(mtx_);
	auto it = socket_to_uid_.find(sock);// 查找Socket对应的UID，找到it = {sock, uid}，否则it = socket_to_uid_.end()
	if (it != socket_to_uid_.end()) {
		uid_to_socket_.erase(it->second);// 根据UID删除Socket记录
		socket_to_uid_.erase(it);// 根据Socket删除UID记录
	}
}

SOCKET ConnectionManager::get_socket(int uid) const {
	std::lock_guard<std::mutex> lock(mtx_);
	auto it = uid_to_socket_.find(uid);// 查找UID对应的Socket，找到it = {uid, sock}，否则it = uid_to_socket_.end()
	return it != uid_to_socket_.end() ? it->second : INVALID_SOCKET;
}

int ConnectionManager::get_uid(SOCKET sock) const {
	std::lock_guard<std::mutex> lock(mtx_);
	auto it = socket_to_uid_.find(sock);// 查找Socket对应的UID，找到it = {sock, uid}，否则it = socket_to_uid_.end()
	return it != socket_to_uid_.end() ? it->second : -1;
}

bool ConnectionManager::is_connected(int uid) const {
	return get_socket(uid) != INVALID_SOCKET;
}
