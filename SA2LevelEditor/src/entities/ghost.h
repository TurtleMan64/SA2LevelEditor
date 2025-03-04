#ifndef GHOST_H
#define GHOST_H

#include "dummy.h"

/// <summary>
/// Represents a user created annotation that can be left on the level arbitrarily.
/// For now only sets a color to the base model, but might be extended for text 
/// annotations, more visual settings, etc.
/// </summary>
class Ghost : public Dummy
{
public:
    Ghost(std::list<TexturedModel*>* models, Vector3f* color);
};

#endif