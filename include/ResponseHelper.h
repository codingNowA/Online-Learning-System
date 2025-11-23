#pragma once
#include "crow.h"

// 构造带 CORS 头的响应(暂时没有理解)
inline crow::response withCORS(int code, const std::string& body) {
    crow::response res(code);
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
    res.body = body;
    res.end();
    return res;
}
