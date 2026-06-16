#pragma execution_character_set("utf-8")
#define _CRT_SECURE_NO_WARNINGS
#include "tables.h"
#include "DB_helper.h"
#include "order.h"
#include <iostream>
#include <cstdio>
using namespace std;

//展示所有餐桌信息
void tablesManager::showTablesInfo() {
	char sql[512];
	sprintf_s(sql, sizeof(sql), "select * from table_item");
	MYSQL_RES* res = db.querySQL(sql);
	if (!res) {
		cout << "目前没有餐桌信息!请先设置餐桌信息!" << endl;
		return;
	}
	MYSQL_ROW row;
	cout << "\t餐桌信息:" << endl;
	cout << "餐桌号\t订单号\t占用情况\t开始占用时间" << endl;
	while ((row = mysql_fetch_row(res))) {
		const char* order_id    = row[1] ? row[1] : "NULL";
		const char* occupy_time = row[3] ? row[3] : "NULL";
		const char* end_occupy  = row[4] ? row[4] : "NULL";

		char occupy_situ[10];
		if (row[2] && strcmp(row[2], "0") == 0)
			sprintf_s(occupy_situ, sizeof(occupy_situ), "No");
		else
			sprintf_s(occupy_situ, sizeof(occupy_situ), "Yes");

		cout << row[0] << "\t" << order_id << "\t" << occupy_situ << "\t" << occupy_time << endl;
	}
	mysql_free_result(res);
}

//添加新餐桌
bool tablesManager::AddNewTable(int table_id) {
	char sql[512];
	sprintf_s(sql, sizeof(sql), "insert into table_item(table_id) values(%d)", table_id);
	if (!db.executeSQL(sql)) {
		cout << "该餐桌已存在!" << endl;
		return false;
	}
	cout << "新餐桌添加成功!" << endl;
	return true;
}

//去除现有餐桌
bool tablesManager::RemoveTable(int table_id) {
	char sql[512];
	sprintf_s(sql, sizeof(sql), "delete from table_item where table_id = %d", table_id);
	if (!db.executeSQL(sql)) {
		cout << "删除失败,餐桌不存在!" << endl;
		return false;
	}
	cout << "餐桌删除成功!" << endl;
	return true;
}

//寻找空餐桌
void tablesManager::SearchIdleTable() {
	char sql[512];
	sprintf_s(sql, sizeof(sql), "select table_id from table_item where is_occupy = 0");
	MYSQL_RES* res = db.querySQL(sql);
	if (!res) {
		cout << "当前没有空闲餐桌!请稍后再试!" << endl;
		return;
	}
	MYSQL_ROW row;
	int col_amount = 0;
	while ((row = mysql_fetch_row(res))) {
		cout << row[0] << "\t";
		col_amount++;
		if (col_amount % 5 == 0) cout << endl;//每输出5个餐桌信息就换行，比较美观
	}
	mysql_free_result(res);
}

//占据餐桌
bool tablesManager::OccupyTable(int table_id, int order_id) {
	char sql[512];
	sprintf_s(sql, sizeof(sql), "update table_item set order_id = %d, is_occupy = 1, occupy_time = Now() where table_id = %d", order_id, table_id);
	if (!db.executeSQL(sql)) {
		cout << "未知错误,请稍后再试!" << endl;
		return false;
	}
	cout << "餐桌号: " << table_id << " 开始占用!" << endl;
	return true;
}

//清理餐桌
bool tablesManager::ClsTable(int table_id) {
	char sql[512];
	sprintf_s(sql, sizeof(sql), "update table_item set order_id = NULL, is_occupy = 0, occupy_time = NULL where table_id = %d", table_id);
	if (!db.executeSQL(sql)) {
		cout << "餐桌没被占用或不存在,请稍后再试!" << endl;
		return false;
	}
	cout << "餐桌号: " << table_id << " 已经清理!" << endl;
	return true;
}