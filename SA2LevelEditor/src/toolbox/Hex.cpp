#include <string>
#include <iomanip>
#include <sstream>
#include <cctype>

#include "hex.h"

int Hex::stoh(std::string s)
{
    return std::stoi(s, nullptr, 16);
}

std::string Hex::to_string(int num)
{
    std::stringstream stream;
    stream << std::hex << num;
    std::string result = stream.str();
    for (int i = 0; i < result.length(); i++)
    {
        result[i] = (char)toupper(result[i]);
    }

    if (result.length() % 2 == 1)
    {
        result = "0"+result;
    }

    return "0x"+result;
}
