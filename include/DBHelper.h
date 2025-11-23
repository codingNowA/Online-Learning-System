#pragma once
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <memory>

//用于实现用teacher 123456 用户连接online_learning 数据库
class DBHelper {
public:
    static std::unique_ptr<sql::Connection> getConnection(
        const std::string& user = "teacher",
        const std::string& pass = "123456",
        const std::string& host = "tcp://127.0.0.1:3306"
    ) {
        sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
        return std::unique_ptr<sql::Connection>(driver->connect(host, user, pass));
    }
};
