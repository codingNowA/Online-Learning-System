#include "crow.h"
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include "../include/UserRoleChecker.h"

int main() {
    crow::SimpleApp app;

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
            return crow::response(400, "Invalid JSON");
        }

        std::string courseName = body["name"].s();
        std::string teacher = body["teacher"].s();

        //检测用户身份
        std::string role=UserRoleChecker::getUserRole(teacher);
        if(role!="teacher")
        {
            return crow::response(403,"Only teachers can create courses");
        }

        try {
            sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
            std::unique_ptr<sql::Connection> con(driver->connect("tcp://127.0.0.1:3306", "teacher", "123456"));
            //3306是MySQL默认端口
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
            return crow::response(400, "Invalid JSON");
        }

        std::string student = body["student"].s();
        
        //检测用户身份
        std::string role=UserRoleChecker::getUserRole(student);
        if(role!="student")
        {
            return crow::response(403,"Only students can enroll courses");
        }

        try {
            sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
            std::unique_ptr<sql::Connection> con(
                driver->connect("tcp://127.0.0.1:3306", "teacher", "123456")
            );
            con->setSchema("online_learning");

            std::unique_ptr<sql::PreparedStatement> pstmt(
                con->prepareStatement("INSERT INTO enrollments(course_id, student) VALUES(?, ?)")
            );
            pstmt->setInt(1, courseId);
            pstmt->setString(2, student);
            pstmt->execute();

            return crow::response(200, "Enrollment successful!");
        } catch (sql::SQLException& e) {
            return crow::response(500, std::string("Database error: ") + e.what());
        }
    });


    /****************************************************************** */

    /*
        实现功能：老师布置作业
        coder：ZHW
        测试：postman发送json
            {
            "course_id": 1,
            "teacher":"HW",
            "title": "C++ Homework 1",
            "description": "Implement a simple class",
            "due_date": "2025-12-01"
            }
    */
        CROW_ROUTE(app, "/assignment/create").methods("POST"_method)
    ([](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "Invalid JSON");

        int courseId = body["course_id"].i();
        std::string teacher=body["teacher"].s();
        std::string title = body["title"].s();
        std::string description = body["description"].s();
        std::string dueDate = body["due_date"].s(); // 格式: YYYY-MM-DD

        //检测用户身份
        std::string role=UserRoleChecker::getUserRole(teacher);
        if(role!="teacher")
        {
            return crow::response(403,"Only teachers can post assignments");
        }

        try {
            sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
            std::unique_ptr<sql::Connection> con(driver->connect("tcp://127.0.0.1:3306", "teacher", "123456"));
            con->setSchema("online_learning");

            std::unique_ptr<sql::PreparedStatement> pstmt(
                con->prepareStatement("INSERT INTO assignments(course_id, teacher, title, description, due_date) VALUES(?, ? , ?, ?, ?)")
            );
            pstmt->setInt(1, courseId);
            pstmt->setString(2,teacher);
            pstmt->setString(3, title);
            pstmt->setString(4, description);
            pstmt->setString(5, dueDate);
            pstmt->execute();

            return crow::response(200, "Assignment created successfully!");
        } catch (sql::SQLException& e) {
            return crow::response(500, std::string("Database error: ") + e.what());
        }
    });
    
    /********************************************** */

    /*
        实现功能：学生提交作业
        coder：ZHW
        测试：POST json
            {
            "student": "小明",
            "content": "Here is my homework solution..."
            }
    */
    CROW_ROUTE(app, "/assignment/<int>/submit").methods("POST"_method)
    ([](const crow::request& req, int assignmentId) {
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "Invalid JSON");

        std::string student = body["student"].s();
        std::string content = body["content"].s();

        try {
            sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
            std::unique_ptr<sql::Connection> con(driver->connect("tcp://127.0.0.1:3306", "teacher", "123456"));
            con->setSchema("online_learning");

            std::unique_ptr<sql::PreparedStatement> pstmt(
                con->prepareStatement("INSERT INTO submissions(assignment_id, student, content) VALUES(?, ?, ?)")
            );
            pstmt->setInt(1, assignmentId);
            pstmt->setString(2, student);
            pstmt->setString(3, content);
            pstmt->execute();

            return crow::response(200, "Submission successful!");
        } catch (sql::SQLException& e) {
            return crow::response(500, std::string("Database error: ") + e.what());
        }
    });


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
            std::unique_ptr<sql::Connection> con(driver->connect("tcp://127.0.0.1:3306", "teacher", "123456"));
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

    /*******************************/


    /*
        实现功能:教师给作业评级以及做评价
        coder:ZHW
        测试：
        {
            "teacher":"HW",
            "grade":"A+",
            "comments":"Good job!!!"
        }
    */
    CROW_ROUTE(app, "/assignment/<int>/grade").methods("POST"_method)
    ([](const crow::request& req, int submissionId) {
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "Invalid JSON");

        std::string teacher = body["teacher"].s();   // 教师用户名
        std::string grade = body["grade"].s();         // 级别或分数
        std::string comments = body["comments"].s();   // 评语

        //检测用户身份 
        std::string role=UserRoleChecker::getUserRole(teacher); 
        if(role!="teacher") 
        { 
            return crow::response(403,"Only teachers can upgrade submissions"); 
        }

        try {
            sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
            std::unique_ptr<sql::Connection> con(driver->connect("tcp://127.0.0.1:3306", "teacher", "123456"));
            con->setSchema("online_learning");

            

            // 更新成绩和反馈
            std::unique_ptr<sql::PreparedStatement> pstmt(
                con->prepareStatement("UPDATE submissions SET grade=?, comments=? WHERE id=?")
            );
            pstmt->setString(1, grade);
            pstmt->setString(2, comments);
            pstmt->setInt(3, submissionId);
            pstmt->execute();

            return crow::response(200, "Grading successful!");
        } catch (sql::SQLException& e) {
            return crow::response(500, std::string("Database error: ") + e.what());
        }
    });




    /********************************* */

    /*
        下面执行运行操作
    */
    app.port(18080).multithreaded().run();
    //这个程序负责接待从 18080 端口发过来的请求,相当于是我的设备变成了一个服务器，在不断监听
}
