#pragma once
#include "crow.h"
#include "UserRoleChecker.h"
#include "DBHelper.h"
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include "Utils.h" // 实现了 urlDecode
#include <fstream>
#include <filesystem>
#include <regex>
#include <chrono>
#include <sstream>

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
        功能: 接收教师上传的文件（multipart/form-data），保存到服务器本地并在 materials 表中创建记录
        POST /course/<int>/material/upload
        form fields: file (binary), teacher, title
    */
    CROW_ROUTE(app, "/course/<int>/material/upload").methods("POST"_method, "OPTIONS"_method)
    ([&app](const crow::request& req, int courseId) {
        // 仅支持 multipart/form-data
        auto contentType = req.get_header_value("Content-Type");
        auto pos = contentType.find("boundary=");
        if (pos == std::string::npos) return crow::response(400, "Missing boundary in Content-Type");
        std::string boundary = contentType.substr(pos + 9); // after 'boundary='
        if (boundary.size() == 0) return crow::response(400, "Invalid boundary");
        std::string fullBoundary = "--" + boundary;

        const std::string& body = req.body;

        // 找到文件块: 简单解析，只处理第一个 file 字段
        size_t idx = body.find(fullBoundary);
        if (idx == std::string::npos) return crow::response(400, "Invalid multipart body");
        // 跳过第一个 boundary
        idx += fullBoundary.size();
        if (body.substr(idx, 2) == "\r\n") idx += 2;

        // 解析头部到 \r\n\r\n
        size_t header_end = body.find("\r\n\r\n", idx);
        if (header_end == std::string::npos) return crow::response(400, "Malformed multipart: no header end");
        std::string headers = body.substr(idx, header_end - idx);

        // 查找 filename
        std::smatch m;
        std::string filename;
        std::regex re_fname("filename=\"([^\"]+)\"");
        if (std::regex_search(headers, m, re_fname)) {
            filename = m[1];
        }

        // 查找字段名，支持 file 字段或其他顺序
        // 寻找下一个 boundary 以确定数据结束
        size_t data_start = header_end + 4;
        size_t next_boundary = body.find(fullBoundary, data_start);
        if (next_boundary == std::string::npos) return crow::response(400, "Malformed multipart: no next boundary");

        // 数据块末尾通常有 \r\n 前导，去掉尾部的 \r\n
        size_t data_end = next_boundary;
        if (data_end >= 2 && body[data_end - 2] == '\r' && body[data_end - 1] == '\n') data_end -= 2;

        std::string fileData = body.substr(data_start, data_end - data_start);

        if (filename.empty()) return crow::response(400, "No filename provided");

        // sanitize filename: 只保留安全字符
        std::string safeName;
        for (char c : filename) {
            if (std::isalnum((unsigned char)c) || c == '.' || c == '_' || c == '-') safeName.push_back(c);
        }
        if (safeName.empty()) return crow::response(400, "Filename invalid after sanitization");

        // 生成唯一文件名，使用时间戳前缀
        auto now = std::chrono::system_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
        std::ostringstream ssfn;
        ssfn << ms << "_" << safeName;
        std::string serverFilename = ssfn.str();

        // 保存目录：统一保存到项目根目录下的 uploads/materials
        // 逻辑：如果当前路径是 bin 或 src，则向上退一级
        std::filesystem::path projectRoot = std::filesystem::current_path();
        if (projectRoot.filename() == "bin" || projectRoot.filename() == "src") {
            projectRoot = projectRoot.parent_path();
        }
        std::filesystem::path uploadDir = projectRoot / "uploads" / "materials";
        
        std::error_code ec;
        std::filesystem::create_directories(uploadDir, ec);
        if (ec) return crow::response(500, std::string("Failed to create upload directory: ") + ec.message());

        std::filesystem::path filePath = uploadDir / serverFilename;

        // 写文件（二进制）
        try {
            std::ofstream ofs(filePath, std::ios::binary);
            ofs.write(fileData.data(), static_cast<std::streamsize>(fileData.size()));
            ofs.close();
        } catch (...) {
            return crow::response(500, "Failed to write uploaded file");
        }

        // 从 headers 中尝试提取其它字段（例如 teacher, title）
        // 简单地在整个 body 中查找 teacher 和 title 的表单字段值
        auto findFormValue = [&](const std::string& name)->std::string {
            std::string needle = "name=\"" + name + "\"";
            size_t p = body.find(needle);
            if (p == std::string::npos) return std::string();
            // 跳到该字段的 header end
            size_t hstart = body.rfind("\r\n", p);
            size_t hdouble = body.find("\r\n\r\n", p);
            if (hdouble == std::string::npos) return std::string();
            size_t valstart = hdouble + 4;
            size_t valend = body.find(fullBoundary, valstart);
            if (valend == std::string::npos) return std::string();
            size_t vend = valend;
            if (vend >= 2 && body[vend - 2] == '\r' && body[vend - 1] == '\n') vend -= 2;
            return body.substr(valstart, vend - valstart);
        };

        std::string teacher = findFormValue("teacher");
        std::string title = findFormValue("title");
        if (teacher.empty()) teacher = "unknown";
        if (title.empty()) title = safeName;

        // 插入 materials 表，resource_url 存成 /materials/files/<serverFilename>
        try {
            auto con = DBHelper::getConnection();
            con->setSchema("online_learning");

            std::unique_ptr<sql::PreparedStatement> pstmt(
                con->prepareStatement("INSERT INTO materials(course_id, teacher, title, content, resource_url) VALUES(?, ?, ?, ?, ?)")
            );
            pstmt->setInt(1, courseId);
            pstmt->setString(2, teacher);
            pstmt->setString(3, title);
            pstmt->setString(4, "");
            std::string resourceUrl = std::string("/materials/files/") + serverFilename;
            pstmt->setString(5, resourceUrl);
            pstmt->execute();

            crow::json::wvalue res;
            res["resource_url"] = resourceUrl;
            res["filename"] = serverFilename;
            return crow::response(200, res.dump());
        } catch (sql::SQLException& e) {
            return crow::response(500, std::string("Database error: ") + e.what());
        }
    });

    // 提供下载/访问上传文件的路由
    CROW_ROUTE(app, "/materials/files/<string>").methods("GET"_method)
    ([](const crow::request& req, const std::string& serverFilename) {
        // 安全检查：只允许简单文件名
        for (char c : serverFilename) {
            if (!(std::isalnum((unsigned char)c) || c == '.' || c == '_' || c == '-')) {
                return crow::response(400, "Invalid filename");
            }
        }

        // 统一路径逻辑：如果当前路径是 bin 或 src，则向上退一级
        std::filesystem::path projectRoot = std::filesystem::current_path();
        if (projectRoot.filename() == "bin" || projectRoot.filename() == "src") {
            projectRoot = projectRoot.parent_path();
        }
        std::filesystem::path filePath = projectRoot / "uploads" / "materials" / serverFilename;
        
        if (!std::filesystem::exists(filePath)) return crow::response(404, "File not found");

        std::ifstream ifs(filePath, std::ios::binary);
        if (!ifs) return crow::response(500, "Failed to open file");

        std::ostringstream buf;
        buf << ifs.rdbuf();
        std::string content = buf.str();

        // 简单根据扩展名判断 Content-Type
        std::string contentType = "application/octet-stream";
        auto ext = filePath.extension().string();
        if (ext == ".pdf") contentType = "application/pdf";
        else if (ext == ".ppt" || ext == ".pptx") contentType = "application/vnd.ms-powerpoint";
        else if (ext == ".zip") contentType = "application/zip";

        crow::response resp;
        resp.code = 200;
        resp.set_header("Content-Type", contentType);
        resp.set_header("Content-Disposition", std::string("attachment; filename=\"") + serverFilename + "\"");
        resp.write(content);
        return resp;
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
