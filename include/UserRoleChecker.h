#pragma once
#include <string>
#include <memory>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>

class UserRoleChecker {
public:
    // 查询用户角色，返回 "teacher" / "student" / "unknown"
    static std::string getUserRole(const std::string& username,
                                   const std::string& dbUser = "teacher",
                                   const std::string& dbPass = "123456",
                                   const std::string& dbName = "online_learning") 
    {
        try {
            sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
            std::unique_ptr<sql::Connection> con(
                driver->connect("tcp://127.0.0.1:3306", dbUser, dbPass)
            );
            con->setSchema(dbName);

            std::unique_ptr<sql::PreparedStatement> pstmt(
                con->prepareStatement("SELECT role FROM users WHERE username=?")
            );
            pstmt->setString(1, username);

            std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
            if (res->next()) {
                return res->getString("role");
            } else {
                return "unknown"; // 用户不存在
            }
        } catch (sql::SQLException& e) {
            return "error"; // 数据库错误
        }
    }
};
