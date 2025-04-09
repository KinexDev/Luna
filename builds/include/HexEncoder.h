#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include <filesystem>

namespace HexEncoder {
    std::string DecodeFromHex(const std::vector<unsigned char>& hex);
}