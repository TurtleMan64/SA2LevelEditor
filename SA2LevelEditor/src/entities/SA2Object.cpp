#include <glad/glad.h>
#include <cstring>

#include "sa2object.h"


SA2Object::SA2Object()
{
	
}

SA2Object::SA2Object(char data[32])
{
    std::memcpy(rawData, data, 32);
    //objectType = data[0]; //+ (data[1]&0x7)<<8; figure this out later
    //clippingLevel = 0;//data[1]&0x7; something like this
    //rotX = data[2] + (data[3]<<8);
    //rotY = data[4] + (data[5]<<8);
    //rotZ = data[6] + (data[7]<<8);
    //memcpy(&x, &data[8],  4);
    //memcpy(&y, &data[12], 4);
    //memcpy(&z, &data[16], 4);
    //memcpy(&a, &data[20], 4);
    //memcpy(&b, &data[24], 4);
    //memcpy(&c, &data[28], 4);
}

