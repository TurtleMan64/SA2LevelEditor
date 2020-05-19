#include <glad/glad.h>
#include <cstring>

#include "sa2object.h"


SA2Object::SA2Object()
{
    
}

SA2Object::SA2Object(char data[32], bool)
{
    std::memcpy(rawData, data, 32);
}

void SA2Object::updateEditorWindows()
{

}

void SA2Object::updateValue(int)
{

}

void SA2Object::fillData(char data[32])
{
    data;
}

std::string SA2Object::toSabString()
{
    return "";
}

int SA2Object::getSwitchID()
{
    return -1;
}

int SA2Object::getShrineID()
{
    return -1;
}

bool SA2Object::isSA2Object()
{
    return true;
}
