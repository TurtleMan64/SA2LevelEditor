#ifndef RING_H
#define RING_H

class TexturedModel;
class CollisionModel;

#include <list>
#include "../entity.h"
#include "../sa2object.h"


class Ring : public SA2Object
{
private:
	static std::list<TexturedModel*> models;

    static CollisionModel* cmBase;

public:
	Ring();
	Ring(char data[32]);
    //Ring(Vector3f* p);

	void step();

	std::list<TexturedModel*>* getModels();

	static void loadStaticModels();

	static void deleteStaticModels();
};
#endif
