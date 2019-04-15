#ifndef RINGLINEAR_H
#define RINGLINEAR_H

class TexturedModel;
//class CollisionModel;

#include <list>
#include <vector>
#include "../entity.h"
#include "../sa2object.h"
#include "../dummy.h"


class RING_LINEAR : public SA2Object
{
private:
	//static std::list<TexturedModel*> models;

    //static CollisionModel* cmBase;

    std::vector<Dummy*> rings;
    std::vector<CollisionModel*> cms;

    int numRings;
    float ringDelta;

public:
	RING_LINEAR();
	RING_LINEAR(char data[32]);

	void step();

	std::list<TexturedModel*>* getModels();

	static void loadStaticModels();

	static void deleteStaticModels();
};
#endif
