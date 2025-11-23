#include "crow.h"
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include "../include/UserRoleChecker.h"
#include "../include/DBHelper.h"
#include "../include/CourseRoutes.h"
#include "../include/AssignmentRoutes.h"
#include "../include/UserRoutes.h"

int main() {
    crow::SimpleApp app;

    registerCourseRoutes(app);
    registerAssignmentRoutes(app);
    registerUserRoutes(app);

    /*下面执行运行操作*/
    app.port(18080).multithreaded().run();
    //这个程序负责接待从 18080 端口发过来的请求,相当于是我的设备变成了一个服务器，在不断监听
}
