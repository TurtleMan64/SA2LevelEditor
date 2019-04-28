#ifndef RING_H
#define RING_H

class TexturedModel;
class CollisionModel;

#include <list>
#include "../entity.h"
#include "../sa2object.h"

class RING : public SA2Object
{
private:
	static std::list<TexturedModel*> models;

    static CollisionModel* cmBase;

    friend class RING_LINEAR;
    friend class RING_CIRCLE;

public:
	RING();
	RING(char data[32]);
    //Ring(Vector3f* p);

	void step();

    void updateEditorWindows();

    void updateValue(int btnIndex);

    void fillData(char data[32]);

    bool isSA2Object();

	std::list<TexturedModel*>* getModels();

	static void loadStaticModels();

	static void deleteStaticModels();
};
#endif