#pragma once
#ifndef STATISTICS_H
#define STATISTICS_H
#include"DB_helper.h"

class StatisticsManager {
private:
	DBHelper& db;
public:
	StatisticsManager(DBHelper& dbh) :db(dbh) {}
	void showSalesByDish();
	void showDailySales();
};
#endif // !STATISTICS_H
