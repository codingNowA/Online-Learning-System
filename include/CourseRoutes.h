#pragma once
#include "crow.h"
#include "UserRoleChecker.h"
#include "DBHelper.h"
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include "Utils.h" // 实现了 urlDecode

// 实现和课程有关的路由功能
template<typename App> // 为支持不同中间件，使用模板（如 CorsMiddleware）
void registerCourseRoutes(App& app) {
    /*
        实现功能：创建课程, sql 修改 courses 表格
        coder: ZHW
        测试方法：用 postman 模拟 post 请求，发送 JSON
        {
            "name": "xxxxx",
            "teacher": "yyy"
        }
    */

    // 教师创建课程
    CROW_ROUTE(app, "/course/create").methods("POST"_method, "OPTIONS"_method)
    ([](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "Invalid JSON");

        std::string courseName = body["name"].s();
        std::string teacher = body["teacher"].s();

        // 检测用户身份
        std::string role = UserRoleChecker::getUserRole(teacher);
        if (role != "teacher") {
            return crow::response(403, "Only teachers can create courses");
        }

        try {
            auto con = DBHelper::getConnection();
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

    /************************************************************/

    // 学生选课
    CROW_ROUTE(app, "/course/<int>/enroll").methods("POST"_method, "OPTIONS"_method)
    ([](const crow::request& req, int courseId) {
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "Invalid JSON");

        std::string student = body["student"].s();
        std::string role = UserRoleChecker::getUserRole(student);
        if (role != "student") {
            return crow::response(403, "Only students can enroll courses");
        }

        try {
            auto con = DBHelper::getConnection();
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

    /************************************************************/

    // 学生查看自己选的课程
    CROW_ROUTE(app, "/student/<string>/courses").methods("GET"_method, "OPTIONS"_method)
    ([](const crow::request& req, const std::string& raw_student) {
        std::string student = urlDecode(raw_student);

        try {
            auto con = DBHelper::getConnection();
            con->setSchema("online_learning");

            std::unique_ptr<sql::PreparedStatement> pstmt(
                con->prepareStatement(
                    "SELECT c.id, c.name, c.teacher "
                    "FROM enrollments e JOIN courses c ON e.course_id=c.id "
                    "WHERE e.student=?"
                )
            );

            pstmt->setString(1, student);
            std::unique_ptr<sql::ResultSet> resSet(pstmt->executeQuery());

            crow::json::wvalue result;
            int idx = 0;
            while (resSet->next()) {
                result[idx]["id"] = resSet->getInt("id");
                result[idx]["name"] = resSet->getString("name");
                result[idx]["teacher"] = resSet->getString("teacher");
                idx++;
            }

            return crow::response(200, result.dump());
        } catch (sql::SQLException& e) {
            return crow::response(500, std::string("Database error: ") + e.what());
        }
    });

    /************************************************************/

    // 获取当前学生可选的所有课程
    CROW_ROUTE(app, "/student/<string>/available_courses").methods("GET"_method)
    ([](const crow::request& req, const std::string& raw_student) {
        std::string student = urlDecode(raw_student);
        try {
            auto con = DBHelper::getConnection();
            con->setSchema("online_learning");

            std::unique_ptr<sql::PreparedStatement> pstmt(
                con->prepareStatement(
                    "SELECT c.id, c.name, c.teacher "
                    "FROM courses c "
                    "WHERE c.id NOT IN (SELECT e.course_id FROM enrollments e WHERE e.student=?)"
                )
            );
            pstmt->setString(1, student);

            std::unique_ptr<sql::ResultSet> rs(pstmt->executeQuery());

            crow::json::wvalue result;
            int idx = 0;
            while (rs->next()) {
                result[idx]["id"] = rs->getInt("id");
                result[idx]["name"] = rs->getString("name");
                result[idx]["teacher"] = rs->getString("teacher");
                idx++;
            }

            return crow::response(200, result.dump());
        } catch (sql::SQLException& e) {
            return crow::response(500, std::string("Database error: ") + e.what());
        }
    });

    /************************************************************/

    // 老师查看自己任课的课程
    CROW_ROUTE(app, "/teacher/<string>/courses").methods("GET"_method, "OPTIONS"_method)
    ([](const crow::request& req, const std::string& raw_teacher) {
        std::string teacher = urlDecode(raw_teacher);

        try {
            auto con = DBHelper::getConnection();
            con->setSchema("online_learning");

            std::unique_ptr<sql::PreparedStatement> pstmt(
                con->prepareStatement("SELECT id, name FROM courses WHERE teacher=?")
            );
            pstmt->setString(1, teacher);
            std::unique_ptr<sql::ResultSet> resSet(pstmt->executeQuery());

            crow::json::wvalue result;
            int idx = 0;
            while (resSet->next()) {
                result[idx]["id"] = resSet->getInt("id");
                result[idx]["name"] = resSet->getString("name");
                idx++;
            }

            return crow::response(200, result.dump());
        } catch (sql::SQLException& e) {
            return crow::response(500, std::string("Database error: ") + e.what());
        }
    });

    /******************* 课件功能 (Materials) *******************/
    /*
        功能: 教师为课程发布课件 (可包含文本内容或资源 URL)
        POST /course/<int>/material/create
        body: { "teacher": "老师用户名", "title": "标题", "content": "文本内容", "resource_url": "https://..." }
    */
    CROW_ROUTE(app, "/course/<int>/material/create").methods("POST"_method, "OPTIONS"_method)
    ([](const crow::request& req, int courseId) {
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "Invalid JSON");

        std::string teacher = body["teacher"].s();
        std::string title = body["title"].s();
        std::string content = body.has("content") ? body["content"].s() : std::string();
        std::string resource_url = body.has("resource_url") ? body["resource_url"].s() : std::string();

        // 验证身份
        std::string role = UserRoleChecker::getUserRole(teacher);
        if (role != "teacher") {
            return crow::response(403, "Only teachers can publish materials");
        }

        // 可选：校验老师是否属于该课程（可按需启用）
        try {
            auto con = DBHelper::getConnection();
            con->setSchema("online_learning");

            std::unique_ptr<sql::PreparedStatement> pstmt(
                con->prepareStatement("INSERT INTO materials(course_id, teacher, title, content, resource_url) VALUES(?, ?, ?, ?, ?)")
            );
            pstmt->setInt(1, courseId);
            pstmt->setString(2, teacher);
            pstmt->setString(3, title);
            pstmt->setString(4, content);
            pstmt->setString(5, resource_url);
            pstmt->execute();

            return crow::response(200, "Material published successfully!");
        } catch (sql::SQLException& e) {
            return crow::response(500, std::string("Database error: ") + e.what());
        }
    });

    /*
        功能: 获取某课程的所有课件
        GET /course/<int>/materials
    */
    CROW_ROUTE(app, "/course/<int>/materials").methods("GET"_method, "OPTIONS"_method)
    ([](const crow::request& req, int courseId) {
        try {
            auto con = DBHelper::getConnection();
            con->setSchema("online_learning");

            std::unique_ptr<sql::PreparedStatement> pstmt(
                con->prepareStatement("SELECT id, teacher, title, content, resource_url, created_at FROM materials WHERE course_id=? ORDER BY created_at DESC")
            );
            pstmt->setInt(1, courseId);
            std::unique_ptr<sql::ResultSet> rs(pstmt->executeQuery());

            crow::json::wvalue result;
            int idx = 0;
            while (rs->next()) {
                result[idx]["id"] = rs->getInt("id");
                result[idx]["teacher"] = rs->getString("teacher");
                result[idx]["title"] = rs->getString("title");
                result[idx]["content"] = rs->getString("content");
                result[idx]["resource_url"] = rs->getString("resource_url");
                result[idx]["created_at"] = rs->getString("created_at");
                idx++;
            }

            return crow::response(200, result.dump());
        } catch (sql::SQLException& e) {
            return crow::response(500, std::string("Database error: ") + e.what());
        }
    });

    /*
        功能: 教师查看自己发布的课件列表
        GET /teacher/<string>/materials
    */
    CROW_ROUTE(app, "/teacher/<string>/materials").methods("GET"_method, "OPTIONS"_method)
    ([](const crow::request& req, const std::string& raw_teacher) {
        std::string teacher = urlDecode(raw_teacher);
        try {
            auto con = DBHelper::getConnection();
            con->setSchema("online_learning");

            std::unique_ptr<sql::PreparedStatement> pstmt(
                con->prepareStatement("SELECT id, course_id, title, content, resource_url, created_at FROM materials WHERE teacher=? ORDER BY created_at DESC")
            );
            pstmt->setString(1, teacher);
            std::unique_ptr<sql::ResultSet> rs(pstmt->executeQuery());

            crow::json::wvalue result;
            int idx = 0;
            while (rs->next()) {
                result[idx]["id"] = rs->getInt("id");
                result[idx]["course_id"] = rs->getInt("course_id");
                result[idx]["title"] = rs->getString("title");
                result[idx]["content"] = rs->getString("content");
                result[idx]["resource_url"] = rs->getString("resource_url");
                result[idx]["created_at"] = rs->getString("created_at");
                idx++;
            }

            return crow::response(200, result.dump());
        } catch (sql::SQLException& e) {
            return crow::response(500, std::string("Database error: ") + e.what());
        }
    });

}
