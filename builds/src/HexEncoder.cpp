#include "../include/HexEncoder.h"

std::string HexEncoder::DecodeFromHex(const std::vector<unsigned char>& hex)
{
    std::string decodedStr;
    decodedStr.reserve(hex.size());

    for (size_t i = 0; i < hex.size(); ++i)
    {
        decodedStr += static_cast<char>(hex[i]);
    }

    return decodedStr;
}
