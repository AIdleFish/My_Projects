#pragma once
#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "Common.h"

// 4字节长度头 + 消息体，解决TCP粘包/拆包问题，4字节=32位用户可以发送2^32字节数据，大约4G，满足大部分实例
bool recv_all(SOCKET sock, char* buf, int len);// 从Socket精准接收指定字节数
bool send_all(SOCKET sock, const char* buf, int len);// 从Socket精准发送指定字节数
bool recv_packet(SOCKET sock, std::string& out);// 接收一个完整的数据包
bool send_packet(SOCKET sock, const std::string& data);// 发送一个完整的数据包

#endif
