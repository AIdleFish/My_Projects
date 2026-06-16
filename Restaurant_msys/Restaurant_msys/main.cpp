#pragma execution_character_set("utf-8")
#include"ui.h"
#include<iostream>
#include<Windows.h>
using namespace std;

int main(int argc, const char* argv[]) {
	SetConsoleOutputCP(65001);//设置控制台输出为UTF-8编码，支持中文显示
	SetConsoleCP(65001);//设置控制台输入为UTF-8编码，支持中文输入
	Ui app;
	if (!app.init()) {
		cout << "系统初始化失败" << endl;
		return -1;
	}
	app.showMainMenu();
	cout << "系统已退出!" << endl;
	return 0;
}