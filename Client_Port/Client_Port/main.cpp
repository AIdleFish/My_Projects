#include "ClientConfig.h"
#include "TcpClient.h"
#include "MessageBuilder.h"
#include"OnLine.h"
#include<Windows.h>
#include <iostream>
#include <mutex>
#include<chrono>
#include<thread>

static std::mutex g_print_mtx;
static int g_my_uid = 0;
static std::string g_my_username = "";
static std::string g_my_password = "";

static void print_incoming(const std::string& packet) {
	std::lock_guard<std::mutex> lock(g_print_mtx);

	ParsedMessage msg;
	// 先拆包
	if (parse_message(packet, msg)) {
		std::cout << "\n[收到消息] 来自 UID " << msg.sender_uid
			<< " -> UID " << msg.receiver_uid
			<< ": \"" << msg.content << "\"" << std::endl;
	}
	else if (packet.rfind("[服务端]", 0) == 0) {
		std::cout << "\n[服务端] " << packet.substr(8) << std::endl;
	}
	else {
		std::cout  << packet << std::endl;
	}
	std::cout << "> " << std::flush;
}

static void print_menu() {
	std::cout << "\n--- IM 测试客户端 ---" << std::endl;
	std::cout << "当前 UID: " << g_my_uid << std::endl;
	std::cout << "1. 发送消息" << std::endl;
	std::cout << "2. 发送消息给服务器(UID 100000)" << std::endl;
	std::cout << "3. 退出" << std::endl;
	std::cout << "请选择: ";
}

static bool send_im_message(TcpClient& client, int receiver_uid, const std::string& content) {
	std::string raw = build_message(g_my_uid, receiver_uid, content);
	if (!client.send_message(raw)) {
		std::cerr << "发送失败" << std::endl;
		return false;
	}
	std::cout << "[已发送] -> UID " << receiver_uid << ": \"" << content << "\"" << std::endl;
	return true;
}

int main() {
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);
	DBHelper db;
	db.connectDB();
	OnLine Online(db);
	std::cout << "===== IM Client_port 测试客户端 =====" << std::endl;
	std::cout << "请先启动 Im_Service 服务端，再运行本客户端。" << std::endl;
	while (1) {
		std::cout << "\n请输入你的用户名: ";
		std::cin >> g_my_username;
		std::cout << "\n请输入你的密码: ";
		std::cin >> g_my_password;
		if(Online.Register(g_my_username, g_my_password)) break;
		std::cout << "用户名或密码错误" << std::endl;
	}
	std::cout << "登录成功!" << std::endl;
	g_my_uid = stoi(Online.get_uid(g_my_username));
	TcpClient client;
	if (!client.connect_to(SERVER_HOST, SERVER_PORT)) {
		return 1;
	}
	std::string content = "[客户端连接成功]";
	send_im_message(client, 100000, content.c_str());
	client.start_receive(print_incoming);
	std::this_thread::sleep_for(std::chrono::seconds(1));
	while (client.is_connected()) {
		print_menu();
		std::string choice;
		std::cin >> choice;
		if (choice == "3") {
			break;
		}
		else if (choice == "1") {
			int receiver_uid = 0;
			std::cout << "输入\"back\"返回" << std::endl;
			std::cout << "接收方 UID: ";
			if (!(std::cin >> receiver_uid)) continue;
			std::cin.ignore();
			while(1){
				std::this_thread::sleep_for(std::chrono::seconds(1));
				std::cout << "消息内容: ";
				std::string content;
				std::getline(std::cin, content);
				if (content == "back") break;
				if (content.empty()) {
					std::cout << "消息不能为空" << std::endl;
					continue;
				}
				send_im_message(client, receiver_uid, content);
			}
			
		}
		else if (choice == "2") {
			std::cin.ignore();
			std::cout << "输入\"back\"返回" << std::endl;
			std::cout << "发给服务器的消息: ";
			while(1){
				std::this_thread::sleep_for(std::chrono::seconds(1));
				std::cout << "消息内容: ";
				std::string content;
				std::getline(std::cin, content);
				if (content == "back") break;
				if (content.empty()) {
					std::cout << "消息不能为空" << std::endl;
					continue;
				}
				send_im_message(client, 100000, content);
			}
			
		}
		else {
			std::cout << "无效选择" << std::endl;
		}
	}

	client.disconnect();
	std::cout << "客户端已退出" << std::endl;
	return 0;
}
