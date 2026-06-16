#pragma execution_character_set("utf-8")
#define _CRT_SECURE_NO_WARNINGS
#include "order.h"
#include <iostream>
#include <cstdio>
using namespace std;
//创建订单
int OrderManager::createOrder(int table_id) {
	char sql[512];
	sprintf_s(sql, sizeof(sql), "Insert into orders(table_id) values(%d)", table_id);
	if (!db.executeSQL(sql)) {
		cout << "Order creation failed, please try again later" << endl;
		return -1;
	}
	//获取刚才插入信息的order_id
	MYSQL_RES* res = db.querySQL("select last_insert_id()");
	if (!res) return -1;
	MYSQL_ROW row = mysql_fetch_row(res);
	int Order_id = atoi(row[0]);
	mysql_free_result(res);
	return Order_id;
}
//添加菜品
bool OrderManager::AddItemToOrder(int Order_id, int dish_id, int amount) {
	
	char sql[512];

	//先查询订单是否结算
	sprintf_s(sql, sizeof(sql), "select * from orders where order_id = %d and is_check = 0", Order_id);
	MYSQL_RES* res_1 = db.querySQL(sql);
	if (res_1 == NULL || mysql_num_rows(res_1) == 0) {
		cout << "当前菜单已经结算!" << endl;
		mysql_free_result(res_1);
		return false;
	}

	//先查询菜品单价
	sprintf_s(sql, sizeof(sql), "select dish_price, stock, dish_name from dishes where dish_id = %d", dish_id);
	MYSQL_RES* res = db.querySQL(sql);
	if (!res) {
		cout << "Dish not found, please choose another dish" << endl;
		return false;
	}
	MYSQL_ROW row = mysql_fetch_row(res);
	int stock = atoi(row[1]);
	if (stock < amount || stock == 0) {
		cout << "当前菜品库存不足!" << endl;
		cout << row[2] << " 的库存只剩下: " << row[1] << endl;
		mysql_free_result(res);
		return false;
	}
	double price = atof(row[0]);
	mysql_free_result(res);

	//插入订单明细
	sprintf_s(sql, sizeof(sql), "insert into order_item(order_id, dish_id, dish_amount, subtotal) values(%d, %d, %d, %.2f)", Order_id, dish_id, amount, price * (double)amount);
	if (db.executeSQL(sql)) cout << row[2] << "成功添加到订单! " << endl;

	//更新订单总价
	sprintf_s(sql, sizeof(sql), "Update orders set amount = amount + %.2f where order_id = %d", price * (double)amount, Order_id);
	db.executeSQL(sql);

	//扣减库存
	disMgr.updataDishStock(dish_id, -amount);
	return true;
}
//从订单中删除菜品
bool OrderManager::RemoveItemToOrder(int Order_id, int dish_id) {
	char sql[512];
	//先查询订单是否结算
	sprintf_s(sql, sizeof(sql), "select * from orders where order_id = %d and is_check = 0", Order_id);
	MYSQL_RES* res_1 = db.querySQL(sql);
	if (res_1 == NULL || mysql_num_rows(res_1) == 0) {
		cout << "当前菜单已经结算!" << endl;
		mysql_free_result(res_1);
		return false;
	}
	//先查询订单中是否存在这个菜品
	sprintf_s(sql, sizeof(sql), "select * from order_item where order_id = %d AND dish_id = %d", Order_id, dish_id);
	MYSQL_RES* res = db.querySQL(sql);
	if (!res || mysql_num_rows(res) == 0) {
		cout << "该菜品不在该订单中!" << endl;
		mysql_free_result(res);
		return false;
	}
	
	//先更新总价-这个菜品的总价
	MYSQL_ROW row;
	while ((row = mysql_fetch_row(res))) {
		if (!row) {
			cout << "匹配行失败！" << endl;
			mysql_free_result(res);
			return false;
		}
		sprintf_s(sql, sizeof(sql), "update orders set amount = amount - %.2f where order_id = %d", atof(row[4]), Order_id);
		db.executeSQL(sql);

		//放回仓库
		sprintf_s(sql, sizeof(sql), "update dishes set stock = stock + %d where dish_id = %d", atoi(row[3]), dish_id);
		db.executeSQL(sql);

		//再删除订单详细中的对应菜品订单信息
		sprintf_s(sql, sizeof(sql), "delete from order_item where order_id = %d and dish_id = %d", Order_id, dish_id);
		db.executeSQL(sql);
		cout << "成功从订单 " << Order_id << "中删除菜品" << endl;
	}
	mysql_free_result(res);
	return true;
}

//结账
int OrderManager::CloseOrder(int Order_id) {
	char sql[512];
	//先检查订单是否已结账
	sprintf_s(sql, sizeof(sql), "select * from orders where order_id = %d and is_check =0", Order_id);
	MYSQL_RES* res_1 = db.querySQL(sql);
	if (res_1 == NULL || mysql_num_rows(res_1) == 0) {
		cout << "当前订单已结账过，请重新输入!" << endl;
		return 0;
	}
	//生成总订单
	sprintf_s(sql, sizeof(sql), "select dish_name, dish_amount, subtotal from order_item join dishes on order_item.dish_id = dishes.dish_id where order_id = %d", Order_id);
	MYSQL_RES* res = db.querySQL(sql);
	if (!res) {
		cout << "当前订单没有详细信息!" << endl;
		return -1;
	}
	MYSQL_ROW row;
	cout << "菜名\t数量\t总价" << endl;
	while ((row = mysql_fetch_row(res))) {
		cout << row[0] << "\t" << row[1] << "\t" << row[2] << endl;
	}
	mysql_free_result(res);

	//查找总价和餐桌号
	sprintf_s(sql, sizeof(sql), "select amount,table_id from orders where order_id = %d", Order_id);
	MYSQL_RES* res_2 = db.querySQL(sql);
	if (!res_2) return -1;
	MYSQL_ROW row_1 = mysql_fetch_row(res_2);
	cout << "合计: " << row_1[0] << "$" << endl;

	//标记为已结账
	sprintf_s(sql, sizeof(sql), "Update orders set is_check = 1 where order_id = %d", Order_id);
	db.executeSQL(sql);

	mysql_free_result(res_2);
	return atoi(row_1[1]);//返回餐桌号,便于餐桌清理
}
