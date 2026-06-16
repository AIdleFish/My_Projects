#pragma once
#ifndef DISH_H
#define DISH_H
#include"DB_helper.h"

class DishManager {
private:
	DBHelper& db;//不负责连接数据库,直接使用已经创建好的DBHelper对象
public:
	DishManager(DBHelper& dbh) :db(dbh) {};//构造函数,将初始化的对象直接赋值给db
	void addDish(const char* dish_name, double price, const char* category,int stock);//添加菜品
	void showAllDishes();//展示菜单
	bool updataDishStock(int dish_id, int new_amount);//更新仓库
	bool alterDishStock(int dish_id);//更改菜品信息
};
#endif // !DISH_H
