#ifndef STAGEKILLPLANES_H
#define STAGEKILLPLANES_H

class TexturedModel;
class CollisionModel;

#include <list>
#include "entity.h"

class StageKillplanes : public Entity
{
private:
	static std::list<TexturedModel*> models;
    static CollisionModel* collisionModel;

public:
	StageKillplanes();

	void step();

	std::list<TexturedModel*>* getModels();

    static void loadModels(const char* objFolder, const char* objFilename);

	static void deleteStaticModels();
};
#endif
