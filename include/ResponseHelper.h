#pragma once
#include "crow.h"

// 构造带更宽松 CORS 头的响应
inline crow::response withCORS(int code, const std::string& body) {
    crow::response res;
    res.code = code;
    res.body = body;

    // 放宽跨域策略
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS, PATCH");
    res.set_header("Access-Control-Allow-Headers", "*"); // 允许所有自定义头
    res.set_header("Access-Control-Allow-Credentials", "true"); // 如果需要带 cookie

    return res;  // 不要调用 res.end()
}
