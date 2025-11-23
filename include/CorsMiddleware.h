#pragma once
#include "crow.h"

// 一个简单的 CORS 中间件，允许跨域访问
struct CorsMiddleware {
    struct context {};

    void before_handle(crow::request& req, crow::response& res, context& ctx) {
        // 这里什么都不做，防止逻辑冲突
    }

    void after_handle(crow::request& req, crow::response& res, context& ctx) {
        // 1. 统一添加跨域头 (只在这里加，保证唯一性)
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization, X-Requested-With");

        // 2. 如果是 OPTIONS 请求（预检），强制设置状态码为 200
        // 这样即使路由里逻辑没跑通，只要经过这里，浏览器就能收到 200 OK
        if (req.method == crow::HTTPMethod::OPTIONS) {
            res.code = 200;
        }
    }
};