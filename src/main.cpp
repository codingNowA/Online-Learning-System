#include "crow.h"
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>

int main() {
    crow::SimpleApp app;

    /*
        实现功能：创建课程
        测试方法：用postman模拟post请求，发送JSON
        {
            "name":"xxxxx,
            "teacher":"yyy"
        }
    */

    // 路由：教师创建课程
    //路由，我理解的就是一个ip+port//后面跟着的路径
    //一台电脑由ip确定,电脑上的程序又由port确定，而它又有不同的路由，可以实现不同的功能
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
            std::unique_ptr<sql::Connection> con(driver->connect("tcp://127.0.0.1:3306", "root", "zhw13890566007"));
            con->setSchema("online_learning");

            std::unique_ptr<sql::PreparedStatement> pstmt(
                con->prepareStatement("INSERT INTO courses(name, teacher) VALUES(?, ?)")
            );//这里应该是智能指针的构造函数初始化写法
            pstmt->setString(1, courseName);
            pstmt->setString(2, teacher);
            pstmt->execute();

            return crow::response(200, "Course created successfully!");
        } catch (sql::SQLException& e) {
            return crow::response(500, std::string("Database error: ") + e.what());
        }
    });

    app.port(18080).multithreaded().run();
    //这个程序负责接待从 18080 端口发过来的请求,相当于是我的设备变成了一个服务器，在不断监听

    /************************************** */
}
