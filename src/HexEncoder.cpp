#include "../include/HexEncoder.h"

#include <sstream>

std::string HexEncoder::EncodeToHex(const std::string& str)
{
    std::ostringstream oss;
    for (size_t i = 0; i < str.length(); ++i) 
    {
        oss << "0x";

        oss << std::setw(2) << std::setfill('0') << std::hex
            << (int)(unsigned char)str[i];

        if (i != str.length() - 1) 
        {
            oss << ", ";
        }
    }

    return oss.str();
}

std::string HexEncoder::DecodeFromHex(const int* hex, size_t len)
{
    std::string decodedStr;

    for (size_t i = 0; i < len; ++i)
    {
        decodedStr += static_cast<char>(hex[i]);
    }

    return decodedStr;
}
