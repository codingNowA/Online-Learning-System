#include "crow.h"
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include "../include/UserRoleChecker.h"
#include "../include/DBHelper.h"
#include "../include/CourseRoutes.h"
#include "../include/AssignmentRoutes.h"
#include "../include/UserRoutes.h"
#include "../include/CorsMiddleware.h"

int main() 
{
    //crow::SimpleApp app;
    crow::App<CorsMiddleware> app;  // 注意这里启用了 CORS 中间件

    registerCourseRoutes(app);
    registerAssignmentRoutes(app);
    registerUserRoutes(app);

    //下面执行运行操作
    app.port(18080).multithreaded().run();
    //这个程序负责接待从 18080 端口发过来的请求,相当于是我的设备变成了一个服务器，在不断监听
}

/*测试
#include "crow.h"
#include "../include/CorsMiddleware.h"

int main() {
    crow::App<CorsMiddleware> app;  // 启用跨域中间件

    // 根路由，方便测试
    CROW_ROUTE(app, "/").methods("GET"_method)
    ([](){
        return crow::response(200, "Crow server is running!");
    });

    // 一个简单的 POST 路由
    CROW_ROUTE(app, "/hello").methods("POST"_method)
    ([](const crow::request& req){
        return crow::response(200, "Hello World from Crow!");
    });

    app.port(18080).multithreaded().run();
}

*/