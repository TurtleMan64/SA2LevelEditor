#include <glad/glad.h>
#include <cstring>

#include "sa2object.h"


SA2Object::SA2Object()
{
	
}

SA2Object::SA2Object(char data[32], bool /*useDefaultValues*/)
{
    std::memcpy(rawData, data, 32);
    typeRotations = IS_INT;
    typePositions = IS_FLOAT;
    typeVars = IS_FLOAT;
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

int SA2Object::getSwitchID() {
	return -1;
}

int SA2Object::getShrineID() {
	return -1;
}

bool SA2Object::isSA2Object()
{
    return true;
}

int SA2Object::getIntRotationX()
{
    return 0;
}

int SA2Object::getIntRotationY()
{
    return 0;
}

int SA2Object::getIntRotationZ()
{
    return 0;
}

int SA2Object::getIntPositionX()
{
    return 0;
}

int SA2Object::getIntPositionY()
{
    return 0;
}

int SA2Object::getIntPositionZ()
{
    return 0;
}

int SA2Object::getIntVar1()
{
    return 0;
}

int SA2Object::getIntVar2()
{
    return 0;
}

int SA2Object::getIntVar3()
{
    return 0;
}

float SA2Object::getFloatRotationX()
{
    return 0.0f;
}

float SA2Object::getFloatRotationY()
{
    return 0.0f;
}

float SA2Object::getFloatRotationZ()
{
    return 0.0f;
}

float SA2Object::getFloatPositionX()
{
    return 0.0f;
}

float SA2Object::getFloatPositionY()
{
    return 0.0f;
}

float SA2Object::getFloatPositionZ()
{
    return 0.0f;
}

float SA2Object::getFloatVar1()
{
    return 0.0f;
}

float SA2Object::getFloatVar2()
{
    return 0.0f;
}

float SA2Object::getFloatVar3()
{
    return 0.0f;
}

std::string SA2Object::getDescriptionRotationX()
{
    return "";
}

std::string SA2Object::getDescriptionRotationY()
{
    return "";
}

std::string SA2Object::getDescriptionRotationZ()
{
    return "";
}

std::string SA2Object::getDescriptionPositionX()
{
    return "";
}

std::string SA2Object::getDescriptionPositionY()
{
    return "";
}

std::string SA2Object::getDescriptionVar1()
{
    return "";
}

std::string SA2Object::getDescriptionVar2()
{
    return "";
}

std::string SA2Object::getDescriptionVar3()
{
    return "";
}

