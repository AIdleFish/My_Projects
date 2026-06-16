#pragma execution_character_set("utf-8")
#include "ui.h"
#include <iostream>
#include <string>
#include<iomanip>
#define _CRT_SECURE_NO_WARNINGS
#include <conio.h> //_getch()函数调用
using namespace std;

//界面初始化,连接数据库
bool Ui::init() {
	return db.connectDB();
}

//显示主菜单
void Ui::showMainMenu() {
	while (true) {
		system("cls");// 清理界面
		cout << "==== 餐厅管理系统 ====" << endl;
		cout << "1. 菜品管理" << endl;
		cout << "2. 订单管理" << endl;
		cout << "3. 餐桌管理" << endl;
		cout << "4. 数据统计" << endl;
		cout << "0. 退出系统" << endl;
		char ope = _getch();//获取用户选择的操作
		switch (ope) {
		case '1':
			dishMenu();
			break;
		case'2':
			orderMenu();
			break;
		case'3':
			tableMenu();
			break;
		case'4':
			statMenu();
			break;
		case'0':
			return;
		default:
			cout << "不合法输入,请重新输入!" << endl;
			system("pause");
		}
	}
}

//菜单管理界面
void Ui::dishMenu() {
	while (true) {
		system("cls");
		cout << "==== 菜品管理 ====" << endl;
		cout << "1. 添加新菜品" << endl;
		cout << "2. 展示菜单" << endl;
		cout << "3. 设置菜品信息" << endl;
		cout << "0. 返回" << endl;
		char ope = _getch();
		switch (ope) {
		case'1': {
			char dish_name_add[20];
			double dish_price_add;
			char dish_category_add[20];
			int dish_stock_add;
			cout << "输入'back'返回!" << endl;
			cout << "菜名: "; cin >> dish_name_add;
			if (strcmp(dish_name_add, "back")) break;
			cout << "价格: "; cin >> dish_price_add;
			cout << "类别: "; cin >> dish_category_add;
			cout << "库存: "; cin >> dish_stock_add;
			dishMgr.addDish(dish_name_add,  dish_price_add, dish_category_add, dish_stock_add);
			system("pause");
			break;
		}
		case'2': {
			dishMgr.showAllDishes();
			system("pause");
			break;
		}
		case'3': {
			int dish_id_alter;
			cout << "删除菜品请将猜库存设置为0即可" << endl;
			cout << "输入要修改的菜品ID(输入'back'返回): "; cin >> dish_id_alter;
			dishMgr.alterDishStock(dish_id_alter);
			system("pause");
			break;
		}
		case'0': {
			cout << "成功返回主菜单！" << endl;
			return;
		}
		default:
			cout << "不合法输入,请重新输入!" << endl;
			system("pause");
		}
	}
}

//订单管理界面
void Ui::orderMenu() {
	while (true) {
		system("cls");
		cout << "==== 订单管理 ====" << endl;
		cout << "1. 创建订单" << endl;
		cout << "2. 添加菜品到订单" << endl;
		cout << "3. 从订单删除菜品" << endl;
		cout << "4. 订单结账" << endl;
		cout << "0. 返回" << endl;
		char ope = _getch();
		switch (ope) {
		case'1': {
			cout << "--- 创建订单 ---" << endl;
			int table_id;
			cout << "目前的空闲餐桌有: " << endl;
			tabMgr.SearchIdleTable();
			cout << "\n请选择餐桌: "; cin >> table_id;
			int order_id = orderMgr.createOrder(table_id);
			if (order_id != -1) {
				cout << "订单创建成功！" << endl;
				cout << "订单ID: " << order_id << endl;
				cout << "桌号: " << table_id << endl;
			}
			tabMgr.OccupyTable(table_id, order_id);
			system("pause");
			break;
		}
		case'2': {
			cout << "--- 添加菜品到订单 ---" << endl;
			cout << "---菜单---" << endl;
			dishMgr.showAllDishes();
			//显示未结账订单

			cout << "---结账订单--- " << endl;
			char sql[512];
			sprintf(sql, "select order_id, table_id, amount from orders where is_check = 0");
			MYSQL_RES* res = db.querySQL(sql);
			if (!res) {
				cout << "没有未结账订单!" << endl;
				mysql_free_result(res);
				system("pause");
				break;
			}
			cout   << "订单ID\t" << "桌号\t"  << "总价" << endl;
			MYSQL_ROW row;
			while ((row = mysql_fetch_row(res))) {
				cout << row[0] << "\t" << row[1] << "\t" << row[2] << endl;
			}
			mysql_free_result(res);

			int order_id;
			int dish_id_int;
			cout << "输入订单ID: "; cin >> order_id;
			cout << "输入'end'结束添加" << endl;
			while (true) {
				string dish_id;
				int dish_amount;
				cout << "输入菜品ID: "; cin >> dish_id;
				if (dish_id == "end") break;
				try {
					dish_id_int = stoi(dish_id);
					cout << "输入菜品数量: "; cin >> dish_amount;
				}
				catch (exception& e) {
					cout << "不合法输入请重新输入！" << e.what() << endl;
					continue;
				}
				orderMgr.AddItemToOrder(order_id, dish_id_int, dish_amount);
			}

			system("pause");
			break;
		}
		case'3': {
			cout << "--- 从订单中删除菜品 ---" << endl;
			int order_id;
			cout << "输入订单ID: "; cin >> order_id;
			int dish_id;
			cout << "输入要删除的菜品ID: "; cin >> dish_id;
			orderMgr.RemoveItemToOrder(order_id, dish_id);
			system("pause");
			break;
		}
		case'4': {
			cout << "--- 订单结账 ---" << endl;

			cout << "未结账订单有: " << endl;
			char sql[512];
			sprintf(sql, "select order_id, table_id, amount from orders where is_check = 0");
			MYSQL_RES* res = db.querySQL(sql);
			if (!res) {
				cout << "没有未结账订单!" << endl;
				system("pause");
				break;
			}
			cout << "订单ID\t桌号\t总价" << endl;
			MYSQL_ROW row;
			while ((row = mysql_fetch_row(res))) {
				cout << row[0] << "\t" << row[1] << "\t" << row[2] << endl;
			}
			mysql_free_result(res);
			string order_id;
			int order_id_int;
			cout << "输入结账的订单ID(输入'back'返回): ";  cin >> order_id;
			if (order_id == "back") break;
			try {
				order_id_int = stoi(order_id);
			}
			catch (exception& e) {
				cout << "不合法输入!请重新输入!" << e.what() << endl;
			}
			int table_id = orderMgr.CloseOrder(order_id_int);
			if (table_id == 0) break;
			cout << "订单结算成功! " << endl;
			tabMgr.ClsTable(table_id);
			system("pause");
			break;
		}
		case'0':
			cout << "返回主菜单!" << endl;
			return;
		default:
			cout << "不合法输入! 请重新输入!" << endl;
			system("pause");
		}
	}
}

//餐桌管理界面
void Ui::tableMenu() {
	while (true) {
		system("cls");
		cout << "==== 餐桌管理 ====" << endl;
		cout << "1. 展示所有餐桌信息" << endl;
		cout << "2. 添加餐桌" << endl;
		cout << "3. 删除餐桌" << endl;
		cout << "4. 查找空餐桌" << endl;
		cout << "0. 返回" << endl;
		char ope = _getch();
		switch (ope) {
		case'1':
			tabMgr.showTablesInfo();
			system("pause");
			break;
		case'2': {
			int table_id_add;
			cout << "输入要添加的新餐桌桌号: "; cin >> table_id_add;
			tabMgr.AddNewTable(table_id_add);
			system("pause");
			break;
		}
		case'3': {
			int table_id_remove;
			cout << "输入要删除的餐桌桌号: "; cin >> table_id_remove;
			tabMgr.RemoveTable(table_id_remove);
			system("pause");
			break;
		}
		case'4':
			tabMgr.SearchIdleTable();
			cout << endl;
			system("pause");
			break;
		case'0':
			cout << "返回主菜单!" << endl;
			return;
		default:
			cout << "不合法输入!请重新输入!" << endl;
			system("pause");
		}
	}
	}

//数据统计界面
void Ui::statMenu() {
	while (true) {
		system("cls");
		cout << "==== 数据统计 ====" << endl;
		cout << "1. 菜品销售统计" << endl;
		cout << "2. 每日销售统计" << endl;
		cout << "0. 返回" << endl;
		char ope = _getch();
		switch (ope) {
		case'1':
			statMgr.showSalesByDish();
			system("pause");
			break;
		case'2':
			statMgr.showDailySales();
			system("pause");
			break;
		case'0':
			cout << "返回主菜单" << endl;
			return;
		default:
			cout << "不合法输入!请重新输入!" << endl;
			system("pause");
		}
	}
	}