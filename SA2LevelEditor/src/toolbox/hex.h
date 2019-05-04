#ifndef HEX_H
#define HEX_H

#include <string>

class Hex
{
public:
    //converts a hex string to an integer 
    static int stoh(std::string s);

    //converts an integer to a string, in hex format
    static std::string to_string(int num);
};

#endif
