#pragma once
#ifndef UI_H
#define UI_H
#include"DB_helper.h"
#include"dish.h"
#include"order.h"
#include"statistics.h"
#include"tables.h"

class Ui {
private:
	DBHelper db;
	DishManager dishMgr;
	OrderManager orderMgr;
	StatisticsManager statMgr;
	tablesManager tabMgr;
public:
	Ui() :dishMgr(db), orderMgr(db, dishMgr), statMgr(db), tabMgr(db, orderMgr) {};
	bool init();//管理员界面初始化
	void showMainMenu();//显示主菜单
	void dishMenu();//获取菜单表
	void orderMenu();//获取订单表
	void statMenu();//获取统计表
	void tableMenu();//获取餐桌信息
};
#endif // !UI_H
