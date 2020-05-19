#include <fstream>
#include <vector>
#include "readbytes.h"

std::vector<char> readAllBytes(char const* filename)
{
    //std::ifstream file;
    //file.open(filename, std::ios::binary);
    //
    //std::vector<char> result;
    //
    //if (file.is_open())
    //{
    //    while (!file.eof())
    //    {
    //        char dat;
    //        file.read(&dat, 1);
    //        result.push_back(dat);
    //    }
    //}
    //
    //return result;

    
    std::ifstream fl(filename, std::ios::binary);
    fl.seekg( 0, std::ios::end );
    size_t len = (size_t)fl.tellg();
    char* ret = new char[len];
    fl.seekg(0, std::ios::beg);
    fl.read(ret, len);
    fl.close();
    
    std::vector<char> result;
    for (int i = 0; i < (int)len; i++)
    {
        result.push_back(ret[i]);
    }
    delete[] ret;
    return result;
    
}
