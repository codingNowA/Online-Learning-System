#include "crow.h"
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>

int main() {
    crow::SimpleApp app;

    // 路由：教师创建课程
    CROW_ROUTE(app, "/course/create").methods("POST"_method)
    ([](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body) {
            return crow::response(400, "Invalid JSON");
        }

        std::string courseName = body["name"].s();
        std::string teacher = body["teacher"].s();

        try {
            sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
            std::unique_ptr<sql::Connection> con(driver->connect("tcp://127.0.0.1:3306", "root", "password"));
            con->setSchema("online_learning");

            std::unique_ptr<sql::PreparedStatement> pstmt(
                con->prepareStatement("INSERT INTO courses(name, teacher) VALUES(?, ?)")
            );
            pstmt->setString(1, courseName);
            pstmt->setString(2, teacher);
            pstmt->execute();

            return crow::response(200, "Course created successfully!");
        } catch (sql::SQLException& e) {
            return crow::response(500, std::string("Database error: ") + e.what());
        }
    });

    app.port(18080).multithreaded().run();
}
