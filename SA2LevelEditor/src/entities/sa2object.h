#ifndef SA2OBJECT_H
#define SA2OBJECT_H

#include "entity.h"
#include "collideableobject.h"

class SA2Object : public CollideableObject
{
public:
    char rawData[32];

	SA2Object();
	SA2Object(char data[32]);
};
#endif
