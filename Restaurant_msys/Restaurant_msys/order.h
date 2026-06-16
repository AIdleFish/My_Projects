#pragma once
#ifndef ORDER_H
#define ORDER_H

#include"DB_helper.h"
#include"dish.h"

class OrderManager {
private:
	DBHelper& db;
	DishManager& disMgr;
public:
	OrderManager(DBHelper& dbh, DishManager& dm):db(dbh), disMgr(dm){}
	int createOrder(int table_id);
	bool AddItemToOrder(int Order_id, int dish_id, int amount);
	bool RemoveItemToOrder(int Order_id, int dish_id);
	int  CloseOrder(int Order_id);//返回餐桌号,便于餐桌清理
};
#endif // !ORDER_H
