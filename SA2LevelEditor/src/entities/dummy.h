#ifndef DUMMY_H
#define DUMMY_H

class Vector3f;
class TexturedModel;

#include <list>
#include <vector>
#include "entity.h"

class Dummy : public Entity
{
private:
	std::list<TexturedModel*>* myModels;

public:
	Dummy(std::list<TexturedModel*>* models);

	void step();

	void setRotation(int xr, int yr, int zr);

	std::list<TexturedModel*>* getModels();

	void setModels(std::list<TexturedModel*>* newModels);
};
#endif