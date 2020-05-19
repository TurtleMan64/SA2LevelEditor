#include <istream>
#include <string>
#include "getline.h"

std::istream& getlineSafe(std::istream& is, std::string& t)
{
    std::string myline;

    if (getline(is, myline))
    {
       if (myline.size() && myline[myline.size()-1] == '\r')
       {
           t = myline.substr(0, myline.size() - 1);
       }
       else
       {
           t = myline;
       }
    }
    else
    {
        t.clear();
    }

    return is;
}
