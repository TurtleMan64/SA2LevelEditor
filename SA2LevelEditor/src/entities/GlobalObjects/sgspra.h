#ifndef SGSPRA_H
#define SGSPRA_H

class TexturedModel;
class CollisionModel;
class Dummy;

#include <list>
#include <vector>
#include "../entity.h"
#include "../sa2object.h"

class SG_SPRA : public SA2Object
{
private:

    std::vector<Dummy*> guides;

    int controlLockTime;
    float power;
    float var3;

    void spawnGuides();
    void despawnGuides();

public:
	SG_SPRA();
	SG_SPRA(char data[32], bool useDefaultValues);

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
