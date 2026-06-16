#include"statistics.h"
#include<iostream>
using namespace std;
//查看菜品销售情况
void StatisticsManager::showSalesByDish() {
	
	char const* sql = R"(
					select dish_name, SUM(dish_amount) as sole_total, SUM(subtotal) as income_total
					From order_item join dishes on order_item.dish_id = dishes.dish_id
					group by dishes.dish_id
					order by income_total DESC)";
					
	//char sql[512];
	//sprintf_s(sql, sizeof(sql), "select dish_name, sum(dish_amount) as sole_total, sum(subtotal) as income_total From order_item join dishes on order_item.item_id = dishes.dish_id group by dish_id order by income_total desc ");
	MYSQL_RES* res = db.querySQL(sql);
	if (!res) {
		cout << "统计失败,请稍后再试！" << endl;
		return;
	}
	cout << "\t---菜品销售统计——" << endl;
	cout << "菜名\t销售量\t收入" << endl;
	MYSQL_ROW row;
	while ((row = mysql_fetch_row(res))) {
		cout << row[0] << "\t" << row[1] << "\t" << row[2] << endl;
	}
	mysql_free_result(res);
}

//查看近期每日销售情况
void StatisticsManager::showDailySales() {
	char const* sql = R"(
					select date(create_time) as date, SUM(amount) as daily_total
					from orders
					where is_check = 1
					group by date(create_time))";
	MYSQL_RES* res = db.querySQL(sql);
	if (!res) {
		cout << "每日统计查询失败，当天没有销售订单" << endl;
		return;
	}
	cout << "\t---每日销售情况---" << endl;
	cout << "日期\t\t收入" << endl;
	MYSQL_ROW row;
	while ((row = mysql_fetch_row(res))) {
		cout << row[0] << "\t" << row[1] << endl;
	}
	mysql_free_result(res);
}