#include "MessageService.h"
#include "UserManager.h"
#include <iostream>

static const int SERVER_UID = 100000;// 接收方UID位100000时服务器接收消息

// 表示 [接收]( 和 )[发送](
// 可以直接写中文，基于兼容问题写16进制转换
static const std::string TAG_RECV = "\x5b\xe6\x8e\xa5\xe6\x94\xb6\x5d\x28";// [接收] (
static const std::string TAG_SEND = "\x29\x5b\xe5\x8f\x91\xe9\x80\x81\x5d\x28";//  )[发送](

// 将原始消息解析为[接收](发送方UID)[发送](接收方UID)"消息内容"格式
static bool parse_im_message(const std::string& raw, ParsedMessage& out) {// 解析消息
	if (raw.compare(0, TAG_RECV.size(), TAG_RECV) != 0) return false;// 和[接收] ( 比较，如果不一样返回0，表示不合法

	// p1是第一个)下标
	size_t p1 = raw.find(')', TAG_RECV.size());// 从[接收] ( 后开始查找，找到第一个)，记录其下标
	if (p1 == std::string::npos) return false;// 如果没找到，解析错误

	std::string sender = raw.substr(TAG_RECV.size(), p1 - TAG_RECV.size());// 从[接收] (开始赋值，到)前的内容，即UID

	if (raw.compare(p1, TAG_SEND.size(), TAG_SEND) != 0) return false;// 从第一个)开始和)[发送](比较，不一样就解析失败
	
	// p3是第二个)下标
	size_t p3 = raw.find(')', p1 + TAG_SEND.size());// 从)[发送](后面开始查找第二个)，记录其下标
	if (p3 == std::string::npos) return false;

	std::string receiver = raw.substr(p1 + TAG_SEND.size(), p3 - (p1 + TAG_SEND.size()));// 从)[发送](开始赋值，到)前的内容，即UID

	if (p3 + 1 >= raw.size() || raw[p3 + 1] != '"') return false;// 如果第二个)在"后面，则解析错误
	size_t p4 = raw.find('"', p3 + 2);// 找第二个"，从第一个"后开始找
	if (p4 == std::string::npos || p4 + 1 != raw.size()) return false;// 没找到则解析失败

	std::string content = raw.substr(p3 + 2, p4 - p3 - 2);// 从第一个"后开始赋值到第二个"前

	try {
		// 将字符串UID转Int存入out结构体中
		out.sender_uid = std::stoi(sender);
		out.receiver_uid = std::stoi(receiver);
	}
	catch (...) {
		return false;
	}
	out.content = content;
	out.valid = true;// 解析成功
	return true;
}

MessageService::MessageService(DBHelper& db, ConnectionManager& conn_mgr, LogManager& log_mgr)
	: db_(db), conn_mgr_(conn_mgr), log_mgr_(log_mgr) {}

bool MessageService::parse_message(const std::string& raw, ParsedMessage& out) {
	return parse_im_message(raw, out);
}

// 构造消息解析格式，用于将原始消息解析成目标消息赋值到out中，配合Parse_message模块使用
// 即[接收](发送方UID)[发送](接收方UID)"消息内容"格式
std::string MessageService::build_message(int sender_uid, int receiver_uid, const std::string& content) {
	return TAG_RECV + std::to_string(sender_uid) + TAG_SEND
		+ std::to_string(receiver_uid) + ")\"" + content + "\"";
}

// 接收方不在线，把消息保存到数据库
bool MessageService::save_offline_msg(int sender_uid, int receiver_uid, const std::string& content) {
	std::string sql = "INSERT INTO offline_msg(recv_user, send_user, msg) VALUES('"+ std::to_string(receiver_uid) + "','"+ std::to_string(sender_uid) + "','"+ db_.escape(content) + "')";// escape数据库转义函数，处理单引号、特殊字符，防止 SQL 注入漏洞
	return db_.ExecuteSQL(sql.c_str());
}

// 用户上线，将离线消息推送
void MessageService::deliver_offline_messages(int uid, SOCKET sock) {
	std::string sql = "SELECT id, send_user, msg FROM offline_msg WHERE recv_user='"
		+ std::to_string(uid) + "'";
	MYSQL_RES* res = db_.QuerySQL(sql.c_str());
	if (!res) return;// 没有离线消息就返回

	std::vector<int> delivered_ids;
	MYSQL_ROW row;
	while ((row = mysql_fetch_row(res))) {// 遍历所有离线消息
		int msg_id = atoi(row[0]);// 消息id
		int sender_uid = atoi(row[1]);// 发送方UID
		std::string msg_content = row[2] ? row[2] : "";// 消息内容
		std::string fwd = build_message(sender_uid, uid, msg_content);// 构建消息解析结构
		if (send_packet(sock, fwd)) {// 发送数据包
			delivered_ids.push_back(msg_id);// 已发送的消息id存入下标数组
			std::lock_guard<std::mutex> lock(cout_mtx_);
			std::cout << "[离线消息递送] UID " << uid << " <- " << sender_uid
				<< ": " << msg_content << std::endl;
		}
	}
	mysql_free_result(res);

	// 根据已经递送的离线消息id删除对应的数据库记录
	for (int id : delivered_ids) {
		std::string del = "DELETE FROM offline_msg WHERE id=" + std::to_string(id);
		db_.ExecuteSQL(del.c_str());
	}
}

// 客户在线直接递送消息
void MessageService::on_user_online(int uid, SOCKET sock) {
	UserManager user_mgr(db_);// 用户管理模块，直接接入数据库管理
	user_mgr.set_status(uid, 1);// 用户上线，更新在线状态
	conn_mgr_.register_user(uid, sock);// 将用户UID和Socket添加到哈希表中，表示在线
	deliver_offline_messages(uid, sock);// 递送离线消息
}

// 用户离线
void MessageService::on_user_offline(int uid) {
	UserManager user_mgr(db_);// 用户管理模块，直接接入数据库管理
	user_mgr.set_status(uid, 0);// 用户离线，更新在线状态
}

// 处理消息
void MessageService::handle_message(const std::string& raw, SOCKET client_socket) {
	ParsedMessage msg;// 解析完后的消息保存在这里
	if (!parse_message(raw, msg)) {
		std::lock_guard<std::mutex> lock(cout_mtx_);
		std::cerr << "[错误]消息格式不合法: " << raw << std::endl;
		send_packet(client_socket, "[服务端]不合法消息格式");
		return;
	}

	// 消息合法
	UserManager user_mgr(db_);// 用户管理模块，直接接入数据库管理

	if (!user_mgr.user_exists(msg.sender_uid)) {
		send_packet(client_socket, "[服务端]发送方未注册");
		return;
	}

	log_mgr_.add(raw, std::to_string(msg.sender_uid), // 添加日志
		std::to_string(msg.receiver_uid), msg.content);

	on_user_online(msg.sender_uid, client_socket);// 如果用户在线直接转发

	{
		std::lock_guard<std::mutex> lock(cout_mtx_);
		std::cout << "[接收]\t" << raw << std::endl;
	}

	if (msg.receiver_uid == SERVER_UID) {
		std::lock_guard<std::mutex> lock(cout_mtx_);
		std::cout << "\xe6\x8e\xa5\xe6\x94\xb6\xe6\x9d\xa5\xe8\x87\xaa(" << msg.sender_uid
			<< ")\xe7\x9a\x84\xe6\xb6\x88\xe6\x81\xaf\xef\xbc\x9a\""
			<< msg.content << "\"" << std::endl;
		send_packet(client_socket, "[服务端]已读");// 发送回发送方表示收到
		return;
	}

	if (!user_mgr.user_exists(msg.receiver_uid)) {
		send_packet(client_socket, "[服务端]接收方未注册");
		return;
	}

	// 构建消息解析类型
	std::string fwd = build_message(msg.sender_uid, msg.receiver_uid, msg.content);
	SOCKET recv_sock = conn_mgr_.get_socket(msg.receiver_uid);// 根据接收方UID找Socket
	int status = user_mgr.get_status(msg.receiver_uid);// 获取接收方在线状态

	if (recv_sock != INVALID_SOCKET && status == 1) {// 接收方通信正常
		if (send_packet(recv_sock, fwd)) {
			std::lock_guard<std::mutex> lock(cout_mtx_);
			std::cout << "[转发]\t" << msg.sender_uid << " -> "
				<< msg.receiver_uid << ": " << msg.content << std::endl;
			send_packet(client_socket, "[服务端]发送成功");
		}
		else {
			conn_mgr_.unregister_socket(recv_sock);// 如果接收方离线，则删除其在哈希表的UID和Socket数据
			user_mgr.set_status(msg.receiver_uid, 0);
			save_offline_msg(msg.sender_uid, msg.receiver_uid, msg.content);// 将该消息保存在离线表中
			send_packet(client_socket, "[服务端]发送成功");
		}
	}
	else {
		save_offline_msg(msg.sender_uid, msg.receiver_uid, msg.content);
		std::lock_guard<std::mutex> lock(cout_mtx_);
		std::cout << "[离线存储] " << msg.sender_uid << " -> "
			<< msg.receiver_uid << ": " << msg.content << std::endl;
		send_packet(client_socket, "[服务端]发送成功");
	}
}
