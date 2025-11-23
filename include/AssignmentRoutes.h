#pragma once
#include "crow.h"
#include "UserRoleChecker.h"
#include "DBHelper.h"
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include "Utils.h"

// 注册作业相关路由
template<typename App>//为支持不同中间件，使用模板（如CorsMiddleware）
void registerAssignmentRoutes(App& app) {
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
        CROW_ROUTE(app, "/assignment/create").methods("POST"_method, "OPTIONS"_method)
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
            "assignment_id":"1",
            "title":"C++ Homework 1",
            "content": "Here is my homework solution..."
            }
    */
    CROW_ROUTE(app, "/assignment/<string>/submit").methods("POST"_method, "OPTIONS"_method)
    ([](const crow::request& req, const std::string& raw_student) {
        std::string student=urlDecode(raw_student);
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "Invalid JSON");

        int assignment_id = body["assignment_id"].i();
        std::string title =body["title"].s();
        std::string content = body["content"].s();

        try {
            sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
            auto con=DBHelper::getConnection();
            con->setSchema("online_learning");

            std::unique_ptr<sql::PreparedStatement> pstmt(
                con->prepareStatement("INSERT INTO submissions(assignment_id, title, student, content) VALUES(?, ?, ?, ?)")
            );
            pstmt->setInt(1, assignment_id);
            pstmt->setString(2, title);
            pstmt->setString(3, student);
            pstmt->setString(4, content);
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
    CROW_ROUTE(app, "/assignment/<int>/grade").methods("POST"_method, "OPTIONS"_method)
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



    /******************************** */
    /*
        实现功能：学生查看自己提交的作业
    */

    CROW_ROUTE(app, "/student/<string>/submissions").methods("GET"_method,"OPTIONS"_method)
    ([](const crow::request& req, const std::string& raw_student) {
        std::string student = urlDecode(raw_student);
        try {
            auto con = DBHelper::getConnection();
            con->setSchema("online_learning");

            std::unique_ptr<sql::PreparedStatement> pstmt(
                con->prepareStatement(
                    "SELECT id, assignment_id, title, content, grade, comments "
                    "FROM submissions WHERE student=?"
                )
            );
            pstmt->setString(1, student);
            std::unique_ptr<sql::ResultSet> resSet(pstmt->executeQuery());

            crow::json::wvalue result;
            int idx = 0;
            while (resSet->next()) {
                result[idx]["id"] = resSet->getInt("id");
                result[idx]["assignment_id"] = resSet->getInt("assignment_id");
                result[idx]["title"] = resSet->getString("title");
                result[idx]["content"] = resSet->getString("content");
                result[idx]["grade"] = resSet->getString("grade");
                result[idx]["comments"] = resSet->getString("comments");
                idx++;
            }

            return crow::response(200, result.dump());
        } catch (sql::SQLException& e) {
            return crow::response(500, std::string("Database error: ") + e.what());
        }
    });

/************************************* */
    CROW_ROUTE(app, "/student/<string>/pending_assignments").methods("GET"_method,"OPTIONS"_method)
    ([](const crow::request& req, const std::string& raw_student) {
        std::string student = urlDecode(raw_student);

        try {
            auto con = DBHelper::getConnection();
            con->setSchema("online_learning");

            // 查询学生已选课程的所有作业，排除已提交的
            std::unique_ptr<sql::PreparedStatement> pstmt(
                con->prepareStatement(
                    "SELECT a.id, a.title, a.description, a.due_date, a.course_id "
                    "FROM assignments a "
                    "JOIN enrollments e ON a.course_id = e.course_id "
                    "WHERE e.student=? AND a.id NOT IN (SELECT assignment_id FROM submissions WHERE student=?)"
                )
            );
            pstmt->setString(1, student);
            pstmt->setString(2, student);
            std::unique_ptr<sql::ResultSet> resSet(pstmt->executeQuery());

            crow::json::wvalue result;
            int idx = 0;
            while (resSet->next()) {
                result[idx]["id"] = resSet->getInt("id");
                result[idx]["title"] = resSet->getString("title");
                result[idx]["description"] = resSet->getString("description");
                result[idx]["due_date"] = resSet->getString("due_date");
                result[idx]["course_id"] = resSet->getInt("course_id");
                idx++;
            }

            return crow::response(200, result.dump());
        } catch (sql::SQLException& e) {
            return crow::response(500, std::string("Database error: ") + e.what());
        }
    });



/************************************ */
/*
    实现功能:老师查看自己布置的作业
    coder:ZHW
*/

    CROW_ROUTE(app, "/teacher/<string>/assignments").methods("GET"_method,"OPTIONS"_method)
    ([](const crow::request& req, const std::string& raw_teacher) {
        std::string teacher = urlDecode(raw_teacher); // 解码路径参数

        try {
            auto con = DBHelper::getConnection();
            con->setSchema("online_learning");

            std::unique_ptr<sql::PreparedStatement> pstmt(
                con->prepareStatement(
                    "SELECT id, title, description, due_date FROM assignments WHERE teacher=?"
                )
            );
            pstmt->setString(1, teacher);
            std::unique_ptr<sql::ResultSet> resSet(pstmt->executeQuery());

            crow::json::wvalue result;
            int idx = 0;
            while (resSet->next()) {
                result[idx]["id"] = resSet->getInt("id");
                result[idx]["title"] = resSet->getString("title");
                result[idx]["description"] = resSet->getString("description");
                result[idx]["due_date"] = resSet->getString("due_date");
                idx++;
            }

            return crow::response(200, result.dump());
        } catch (sql::SQLException& e) {
            return crow::response(500, std::string("Database error: ") + e.what());
        }
    });


}
