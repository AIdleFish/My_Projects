#include "TcpServer.h"
#include "ThreadPool.h"
#include "DBHelper.h"
#include "ConnectionManager.h"
#include "LogManager.h"
#include "MessageService.h"
#include "UserManager.h"
#include <iostream>
#include <string>
#include <thread>
#include <Windows.h>

using namespace std;

static void show_main_menu() {
	cout << "\n========== IM服务器主菜单 ==========" << endl;
	cout << "1. 管理用户账号" << endl;
	cout << "2. 查看日志" << endl;
	cout << "0. 退出服务器" << endl;
	cout << "====================================" << endl;
	cout << "请选择: ";
}

static void run_log_menu(LogManager& log_mgr) {
	while (true) {
		cout << "\n--- 消息日志 ---" << endl;
		cout << "当前共 " << log_mgr.count() << " 条日志" << endl;
		cout << "输入 view 查看日志, clear 清空日志, back 返回主菜单" << endl;
		cout << "请输入: ";
		string cmd;
		cin >> cmd;
		if (cmd == "back") return;
		if (cmd == "view") {
			log_mgr.print_all();
		}
		else if (cmd == "clear") {
			log_mgr.clear();
			cout << "日志已清空" << endl;
		}
		else {
			cout << "无效命令" << endl;
		}
	}
}

int main() {
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);
	cout << "=== 多线程高并发即时通讯服务器 ===" << endl;

	DBHelper db;// 数据库实例
	if (!db.connectDB()) {
		cerr << "数据库连接失败，请检查Config.h中的配置及MySQL服务" << endl;
		return 1;
	}
	cout << "数据库连接成功" << endl;

	ConnectionManager conn_mgr;
	LogManager log_mgr;
	MessageService msg_service(db, conn_mgr, log_mgr);
	TcpServer server(msg_service, conn_mgr);

	cout << "输入服务器最大处理线程数: ";
	int max_threads;
	cin >> max_threads;
	if (max_threads <= 0) max_threads = 4;// 输入了负数，默认为4

	ThreadPool thread_pool(max_threads);// 创建线程池，单例

	if (!server.init(SERVER_PORT)) {
		cerr << "服务器初始化失败" << endl;
		return 1;
	}
	// 创建一个线程开始监听
	thread server_thread([&server, &thread_pool]() {
		server.start_listening(thread_pool);
	});

	UserManager user_mgr(db);

	while (true) {
		show_main_menu();
		string choice;
		cin >> choice;
		if (choice == "0") break;
		if (choice == "1") {
			user_mgr.run_menu();
		}
		else if (choice == "2") {
			run_log_menu(log_mgr);
		}
		else {
			cout << "无效选择，请重新输入" << endl;
		}
	}

	cout << "正在关闭服务器..." << endl;
	server.close();
	thread_pool.stop();
	if (server_thread.joinable()) {
		server_thread.join();
	}
	db.closeDB();
	WSACleanup();
	cout << "服务器已退出" << endl;
	return 0;
}
