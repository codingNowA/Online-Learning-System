#pragma once
#include <string>
#include <sstream>
#include <iomanip>

inline std::string urlDecode(const std::string& str) {
    std::ostringstream decoded;
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '%' && i + 2 < str.length()) {
            std::istringstream hex(str.substr(i + 1, 2));
            int val;
            hex >> std::hex >> val;
            decoded << static_cast<char>(val);
            i += 2;
        } else if (str[i] == '+') {
            decoded << ' ';
        } else {
            decoded << str[i];
        }
    }
    return decoded.str();
}
