#include <list>

#include "../toolbox/vector.h"
#include "dummy.h"

Dummy::Dummy(std::list<TexturedModel*>* models)
{
    myModels = models;
    
    visible = true;
}

void Dummy::step()
{

}

void Dummy::setRotation(int xr, int yr, int zr)
{
    rotationX = xr;
    rotationY = yr;
    rotationZ = zr;
}

std::list<TexturedModel*>* Dummy::getModels()
{
    return myModels;
}

void Dummy::setModels(std::list<TexturedModel*>* newModels)
{
    myModels = newModels;
}
