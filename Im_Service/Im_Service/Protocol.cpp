#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Protocol.h"

// 循环接收直到读取到指定长度的字节，解决 TCP 单次 recv 可能只返回部分数据的问题
bool recv_all(SOCKET sock, char* buf, int len) {
	int received = 0;// 已经收到的字节数
	while (received < len) {
		int n = recv(sock, buf + received, len - received, 0);// 记录这次接收到多少字节
		if (n <= 0) return false;// 客户端断开或其他中断，recv返回0或-1
		received += n;// 更新已经接收的数据数
	}
	return true;
}

// 循环发送直到发送完全部字节， 解决 TCP 单次 send 可能只发送部分数据的问题
bool send_all(SOCKET sock, const char* buf, int len) {
	int sent = 0;// 已经发送的字节数
	while (sent < len) {
		int n = send(sock, buf + sent, len - sent, 0);
		if (n <= 0) return false;
		sent += n;// 更新已发送的字节数
	}
	return true;
}

// 接收一个完整数据包，执行解包操作
bool recv_packet(SOCKET sock, std::string& out) {
	char header[HEADER_SIZE] = { 0 };// 4位消息头
	if (!recv_all(sock, header, HEADER_SIZE)) return false;// 接收4位消息头

	// 解析消息体长度
	uint32_t body_len = 0;// 32位无符号整数的消息体长度
	memcpy(&body_len, header, HEADER_SIZE);//将4字节的消息头转换位4字节的无符号整形 char(整数) -> usignint(整数)
	body_len = ntohl(body_len);// 将网络字节序转换为当前 CPU 能正确理解的整数格式

	if (body_len == 0 || body_len > 65536) return false;

	// 接收消息体
	std::vector<char> body(body_len);// 消息体
	if (!recv_all(sock, body.data(), static_cast<int>(body_len))) return false;

	out.assign(body.data(), body_len);// 将 vector 中的原始数据拷贝到引用参数 out 中
	return true;
}

// 发送一个完整数据包，执行打包操作
bool send_packet(SOCKET sock, const std::string& data) {
	uint32_t len = htonl(static_cast<uint32_t>(data.size()));// 获取消息体长度32位
	char header[HEADER_SIZE];// 4字节消息头
	memcpy(header, &len, HEADER_SIZE);// 将消息体长度以32位二进制形式拷贝到header中

	if (!send_all(sock, header, HEADER_SIZE)) return false;// 先发送消息头
	if (data.empty()) return true;
	return send_all(sock, data.c_str(), static_cast<int>(data.size()));// 再发送消息体
}
