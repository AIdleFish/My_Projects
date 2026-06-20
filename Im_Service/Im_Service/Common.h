#pragma once
#ifndef COMMON_H
#define COMMON_H

#ifndef WIN32_LEAN_AND_MEAN// 定义 WIN32_LEAN_AND_MEAN 宏，减少 Windows 头文件的包含，优化编译速度
#define WIN32_LEAN_AND_MEAN// 这个宏会排除一些不常用的 Windows 组件，如 Cryptography、DDE、RPC 等，从而减少编译时间和可执行文件的大小
#endif

#ifndef NOMINMAX // 定义 NOMINMAX 宏，防止 Windows 头文件定义 min 和 max 宏，避免与 C++ 标准库中的 std::min 和 std::max 冲突
#define NOMINMAX // 这个宏会阻止 Windows 头文件定义 min 和 max 宏，从而避免与 C++ 标准库中的 std::min 和 std::max 冲突，确保代码的可移植性和兼容性
#endif

// 包含 Windows 头文件，使用 WinSock2 库进行网络编程
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
// 一些头文件
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <condition_variable>
#include <cstring>
#include <mysql.h>

#define HEADER_SIZE 4 // 消息头大小，表示消息体长度的字节数
#define BUFFER_SIZE 1024 // 缓冲区大小
#define SERVER_PORT 8080 // 服务端口号

#define LOG_INFO(msg) std::cout << "[INFO] " << msg << std::endl // 日志宏定义，输出信息级别的日志
#define LOG_ERROR(msg) std::cerr << "[ERROR] " << msg << std::endl // 日志宏定义，输出错误级别的日志

#endif
