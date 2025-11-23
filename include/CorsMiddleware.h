#pragma once
#include "crow.h"

// 一个简单的 CORS 中间件，允许跨域访问
//我理解的就是允许前端localhost:3000访问后端localhost:18080
struct CorsMiddleware {
    struct context {};

    void before_handle(crow::request& req, crow::response& res, context&) {
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");

        if (req.method == crow::HTTPMethod::Options) {
            res.code = 200;
            res.end();
            return;  // 必须加 return
        }
    }

    void after_handle(crow::request& req, crow::response& res, context&) {
        res.add_header("Access-Control-Allow-Origin", "*");
    }
};