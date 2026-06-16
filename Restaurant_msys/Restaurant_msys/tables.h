#pragma once
#ifndef TABLES_H
#define TABLES_H
#include "DB_helper.h"
#include "order.h"

class tablesManager {
private:
	DBHelper& db;
	OrderManager& ordMgr;
public:
	tablesManager(DBHelper& dbh, OrderManager& om) :db(dbh), ordMgr(om) {};
	void showTablesInfo();
	bool AddNewTable(int table_id);
	bool RemoveTable(int table_id);
	void SearchIdleTable();//查找空闲餐桌
	bool OccupyTable(int table_id, int order_id);//占据餐桌
	bool ClsTable(int table_id);//清理餐桌
};
#endif // !TABLES_H
