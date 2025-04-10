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