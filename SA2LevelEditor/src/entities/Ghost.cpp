#include "ghost.h"

Ghost::Ghost(std::list<TexturedModel*>* models, Vector3f* color) :
    Dummy(models)
{
    baseColour.set(color);
}