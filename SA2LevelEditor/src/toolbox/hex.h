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

    //converts a hex string to an short 
    static short stohshort(std::string s);

    //converts an short to a string, in hex format
    static std::string to_string_short(short num);
};

#endif
