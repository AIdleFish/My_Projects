#pragma execution_character_set("utf-8")
#define _CRT_SECURE_NO_WARNINGS
#include "dish.h"
#include "DB_helper.h"
#include <mysql.h>
#include <iostream>
#include <conio.h>
#include <string>
#include <cstdio>
#include<iomanip>
using namespace std;

//添加新菜品
void DishManager::addDish(const char* dish_name, double price, const char* category, int stock) {
    char sql[512];
    sprintf_s(sql, sizeof(sql), "Select * From dishes where dish_name = '%s'", dish_name);
    MYSQL_RES* res = db.querySQL(sql);
    if (res != NULL && mysql_num_rows(res) > 0) {
        cout << dish_name << " 已经存在！" << endl;
        mysql_free_result(res);
        return;
    }

    sprintf_s(sql, sizeof(sql), "Insert into dishes (dish_name, dish_price, dish_category, stock) Values('%s', %.2f, '%s', %d)", dish_name,  price, category, stock);

    if (db.executeSQL(sql)) {
        cout << dish_name << " 添加菜品成功!" << endl;
    }
    else {
        cout << dish_name << " 添加菜品失败!" << endl;
    }
}

// 显示所有菜品
void DishManager::showAllDishes() {
    MYSQL_RES* res = db.querySQL("Select * from dishes");
    if (!res) {
        cout << "菜单是空！" << endl;
        return;
    }
    MYSQL_ROW row;
    cout << left;
    cout  << setw(11) <<"菜品ID" << setw(18) << "菜名" <<setw(10) <<   "价格" << setw(12) <<  "库存" << setw(12) <<  "类别" << endl;
    while ((row = mysql_fetch_row(res))) {
        cout << left;
        cout << setw(9)<< row[0] << setw(20) << row[1] << setw(10) << row[2] << setw(8) << row[4] << setw(12) << row[3] << endl;
    }
    mysql_free_result(res);
}
//点菜后更新仓库
bool DishManager::updataDishStock(int dish_id, int new_amount) {
    char sql[512];
    sprintf_s(sql, sizeof(sql), "Select * from dishes where dish_id = %d", dish_id);
    MYSQL_RES* res = db.querySQL(sql);
    if (res == NULL || mysql_num_rows(res) == 0) {
        cout << "未找到该菜品!" << endl;
        return false;
    }
    mysql_free_result(res);
    sprintf_s(sql, sizeof(sql), "Update dishes set stock = stock + %d where dish_id = %d", new_amount, dish_id);
    return db.executeSQL(sql);
}
//更改菜品信息
bool DishManager::alterDishStock(int dish_id) {
    char sql[512];
    sprintf_s(sql, sizeof(sql), "Select * from dishes where dish_id = %d", dish_id);
    MYSQL_RES* res = db.querySQL(sql);
    if (res == NULL || mysql_num_rows(res) == 0) {
        cout << "未找到该菜品!" << endl;
        return false;
    }
    MYSQL_ROW row = mysql_fetch_row(res);
    char dish_name[64] = {0};
    if (row && row[1]) {
        strncpy(dish_name, row[1], sizeof(dish_name)-1);
    }
    mysql_free_result(res);

    while(true){
        cout << "选择要对" << dish_name << "更改的内容" << endl;
        cout << "1. 菜名" << endl;
        cout << "2. 价格" << endl;
        cout << "3. 类别" << endl;
        cout << "4. 库存" << endl;
        cout << "0. 返回" << endl;
        char ope = _getch();
        switch (ope) {
        case '1': {
            cout << "输入新菜名:" << endl;
            char dish_name_new[20];
            cin >> dish_name_new;
            sprintf_s(sql, sizeof(sql), "update dishes set dish_name = '%s' where dish_id = %d", dish_name_new, dish_id);
            if (db.executeSQL(sql)) cout << "更新成功!" << endl;
            break;
        }
        case '2': {
            cout << "输入新价格:" << endl;
            double price_new;
            cin >> price_new;
            sprintf_s(sql,sizeof(sql), "update dishes set dish_price = %.2f where dish_id = %d", price_new, dish_id);
            if (db.executeSQL(sql)) cout << "更新成功!" << endl;
            break;
        }
        case '3': {
            cout << "输入新类别:" << endl;
            string category_new;
            cin >> category_new;
            sprintf_s(sql, sizeof(sql), "update dishes set dish_category = '%s' where dish_id = %d", category_new.c_str(), dish_id);
            if (db.executeSQL(sql)) cout << "更新成功!" << endl;
            break;
        }
        case '4': {
            cout << "输入新库存:" << endl;
            int stock_new;
            cin >> stock_new;
            sprintf_s(sql, sizeof(sql), "update dishes set stock = %d where dish_id = %d", stock_new, dish_id);
            if (db.executeSQL(sql)) cout << "更新成功!" << endl;
            break;
        }
        case '0':
            cout << "返回" << endl;
            return true;
        default:
            cout << "不合法输入!请重新输入!" << endl;
            system("pause");
        }
    }
    return true;
}
