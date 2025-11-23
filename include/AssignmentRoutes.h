#pragma once
#include "crow.h"
#include "UserRoleChecker.h"
#include "DBHelper.h"
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>

// 注册作业相关路由
void registerAssignmentRoutes(crow::SimpleApp& app) {
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
            auto con=DBHelper::getConnection();
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
            auto con=DBHelper::getConnection();
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
            auto con=DBHelper::getConnection();
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
}
