#pragma once
#include <string>
#include <iostream>
#include <iomanip>

namespace HexEncoder {
    std::string EncodeToHex(const std::string& str);
    std::string DecodeFromHex(const int* hex, size_t len);
}