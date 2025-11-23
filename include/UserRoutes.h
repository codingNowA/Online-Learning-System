#pragma once
#include "crow.h"
#include "DBHelper.h"
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>

// 注册用户相关路由
template<typename App> // 为支持不同中间件，使用模板
inline void registerUserRoutes(App& app) {
    /************************************************************** */

    /*
        实现功能:创建用户
        coder：ZHW
        修改：修复了跨域问题 (CORS) 和 OPTIONS 预检请求失败的问题
        {
            "username": "小明",
            "password": "123456",
            "role": "student"
        }
    */

    // 修改1：在 methods 中显式加入 "OPTIONS"_method
    CROW_ROUTE(app, "/user/create").methods("POST"_method, "OPTIONS"_method)
    ([](const crow::request& req) {
        
        // 修改2：优先处理 OPTIONS 预检请求
        // 浏览器在 POST 之前会发 OPTIONS，此时没有 body，不能解析 json，直接返回 200
        if (req.method == crow::HTTPMethod::OPTIONS) {
            return crow::response(200); 
        }

        // 解析 JSON
        auto body = crow::json::load(req.body);
        
        // 修改3：这里把 withCORS(400, ...) 改成了 crow::response(400, ...)
        // 原因：你的 main 函数里有 CorsMiddleware，它会自动加跨域头。
        // 如果这里用 withCORS 再加一次，浏览器会报 "Multiple CORS header" 错误。
        if (!body) return crow::response(400, "Invalid JSON");

        std::string username = body["username"].s();
        std::string password = body["password"].s();
        std::string role = body["role"].s(); // "teacher" 或 "student"

        try {
            sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
            auto con = DBHelper::getConnection();
            con->setSchema("online_learning");

            std::unique_ptr<sql::PreparedStatement> pstmt(
                con->prepareStatement("INSERT INTO users(username, password, role) VALUES(?, ?, ?)")
            );
            pstmt->setString(1, username);
            pstmt->setString(2, password);
            pstmt->setString(3, role);
            pstmt->execute();

            // 修改4：成功时也使用标准 response
            return crow::response(200, "User created successfully!");
        } catch (sql::SQLException& e) {
            // 修改5：报错时也使用标准 response
            return crow::response(500, std::string("Database error: ") + e.what());
        }
    });


    /************************** */

    /*
        实现功能：用户登录
        coder:ZHW
        测试：
        {
            "username": "小明",
            "password": "123456"
        }
    */

    CROW_ROUTE(app, "/login").methods("POST"_method, "OPTIONS"_method)
    ([](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "Invalid JSON");

        std::string username = body["username"].s();
        std::string password = body["password"].s();

        try {
            auto con = DBHelper::getConnection();
            con->setSchema("online_learning");

            std::unique_ptr<sql::PreparedStatement> pstmt(
                con->prepareStatement("SELECT role FROM users WHERE username=? AND password=?")
            );
            pstmt->setString(1, username);
            pstmt->setString(2, password);

            std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
            if (res->next()) {
                crow::json::wvalue result;
                result["message"] = "Login successful";
                result["role"] = res->getString("role");
                return crow::response(200, result.dump());
            } else {
                return crow::response(401, "Invalid username or password");
            }
        } catch (sql::SQLException& e) {
            return crow::response(500, std::string("Database error: ") + e.what());
        }
    });
}

