#pragma once
#include "crow.h"
#include "UserRoleChecker.h"
#include "DBHelper.h"
#include "ResponseHelper.h"

//实现和课程有关的路由功能
template<typename App>//为支持不同中间件，使用模板（如CorsMiddleware）
void registerCourseRoutes(App& app) {
    /*
        实现功能：创建课程,sql修改courses表格
        coder:ZHW
        测试方法：用postman模拟post请求，发送JSON
        {
            "name":"xxxxx,
            "teacher":"yyy"
        }
    */

    // 路由：教师创建课程
    //路由，我理解的就是一个ip:port//后面跟着的路径
    //一台电脑由ip确定,电脑上的程序又由port确定，而它又有不同的路由，可以实现不同的功能
    CROW_ROUTE(app, "/course/create").methods("POST"_method)
    ([](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body) {
            return withCORS(400, "Invalid JSON");
        }

        std::string courseName = body["name"].s();
        std::string teacher = body["teacher"].s();

        //检测用户身份
        std::string role=UserRoleChecker::getUserRole(teacher);
        if(role!="teacher")
        {
            return withCORS(403,"Only teachers can create courses");
        }

        try {
            sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
            auto con=DBHelper::getConnection();
            //3306是MySQL默认端口
            con->setSchema("online_learning");

            std::unique_ptr<sql::PreparedStatement> pstmt(
                con->prepareStatement("INSERT INTO courses(name, teacher) VALUES(?, ?)")
            );//这里应该是智能指针的构造函数初始化写法
            pstmt->setString(1, courseName);
            pstmt->setString(2, teacher);
            pstmt->execute();

            return withCORS(200, "Course created successfully!");
        } catch (sql::SQLException& e) {
            return withCORS(500, std::string("Database error: ") + e.what());
        }
    });



    /************************************************* */

    /*
        实现功能:学生选课，修改选课表格enrollments
        coder：ZHW
        测试方法：用postman模拟post请求，发送JSON
        {
            "student":"小明";
        }
    */

    CROW_ROUTE(app, "/course/<int>/enroll").methods("POST"_method)
    ([](const crow::request& req, int courseId) {
        //lambda的参数都是从发送请求的URL中得到
        //而"/course/<int>/enroll"更像一个正则匹配的作用，声明截取URL的各部分分别作为指定类型参数
        auto body = crow::json::load(req.body);
        if (!body) {
            return withCORS(400, "Invalid JSON");
        }

        std::string student = body["student"].s();
        
        //检测用户身份
        std::string role=UserRoleChecker::getUserRole(student);
        if(role!="student")
        {
            return withCORS(403,"Only students can enroll courses");
        }

        try {
            sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
            auto con=DBHelper::getConnection();
            con->setSchema("online_learning");

            std::unique_ptr<sql::PreparedStatement> pstmt(
                con->prepareStatement("INSERT INTO enrollments(course_id, student) VALUES(?, ?)")
            );
            pstmt->setInt(1, courseId);
            pstmt->setString(2, student);
            pstmt->execute();

            return withCORS(200, "Enrollment successful!");
        } catch (sql::SQLException& e) {
            return withCORS(500, std::string("Database error: ") + e.what());
        }
    });
}
