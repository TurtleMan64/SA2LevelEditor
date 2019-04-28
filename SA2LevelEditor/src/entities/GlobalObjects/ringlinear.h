#ifndef RINGLINEAR_H
#define RINGLINEAR_H

class TexturedModel;
class CollisionModel;
class Dummy;

#include <list>
#include <vector>
#include "../entity.h"
#include "../sa2object.h"


class RING_LINEAR : public SA2Object
{
private:
    std::vector<Dummy*> rings;
    std::vector<CollisionModel*> cms;

    float ringDelta;
    int numRings;

    void spawnChildren();

public:
	RING_LINEAR();
	RING_LINEAR(char data[32]);

    void updateEditorWindows();

    void updateValue(int btnIndex);

    void fillData(char data[32]);

    bool isSA2Object();

	void step();

	std::list<TexturedModel*>* getModels();

	static void loadStaticModels();

	static void deleteStaticModels();
};
#endif
