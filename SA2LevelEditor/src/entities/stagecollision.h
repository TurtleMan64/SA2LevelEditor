#ifndef STAGECOLLISION_H
#define STAGECOLLISION_H

class TexturedModel;
class CollisionModel;

#include <list>
#include "entity.h"

class StageCollision : public Entity
{
private:
	static std::list<TexturedModel*> models;
    static CollisionModel* collisionModel;

public:
	StageCollision();

	void step();

	std::list<TexturedModel*>* getModels();

    static void loadModels(const char* objFolder, const char* objFilename);

	static void deleteStaticModels();
};
#endif
