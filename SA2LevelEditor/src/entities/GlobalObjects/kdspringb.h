#ifndef KDSPRINGB_H
#define KDSPRINGB_H

class TexturedModel;
class CollisionModel;
class Dummy;

#include <list>
#include <vector>
#include "../entity.h"
#include "../sa2object.h"

class KDSPRINGB : public SA2Object
{
private:
    std::vector<Dummy*> guides;

    int controlLockTime;
    float power;
	int shrineID;

    void spawnGuides();
    void despawnGuides();

public:
	KDSPRINGB();
	KDSPRINGB(char data[32], bool useDefaultValues);

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
