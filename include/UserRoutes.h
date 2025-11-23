#pragma once
#include "crow.h"
#include "DBHelper.h"
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>

// 注册用户相关路由
inline void registerUserRoutes(crow::SimpleApp& app) {
    /************************************************************** */

    /*
        实现功能:创建用户,暂时所有人都可以创建任意用户
        coder：ZHW
        测试：
            {
            "username": "小明",
            "password": "123456",
            "role": "student"
            }

    */

        CROW_ROUTE(app, "/user/create").methods("POST"_method)
    ([](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "Invalid JSON");

        std::string username = body["username"].s();
        std::string password = body["password"].s();
        std::string role = body["role"].s(); // "teacher" 或 "student"

        try {
            sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
            auto con=DBHelper::getConnection();
            con->setSchema("online_learning");

            std::unique_ptr<sql::PreparedStatement> pstmt(
                con->prepareStatement("INSERT INTO users(username, password, role) VALUES(?, ?, ?)")
            );
            pstmt->setString(1, username);
            pstmt->setString(2, password);
            pstmt->setString(3, role);
            pstmt->execute();

            return crow::response(200, "User created successfully!");
        } catch (sql::SQLException& e) {
            return crow::response(500, std::string("Database error: ") + e.what());
        }
    });
}
