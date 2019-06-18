#ifndef KDDRNGC_H
#define KDDRNGC_H

class TexturedModel;
class CollisionModel;
class Dummy;

#include <list>
#include <vector>
#include "../entity.h"
#include "../sa2object.h"


class KDDRNGC : public SA2Object
{
private:
    std::vector<Dummy*> rings;
    std::vector<CollisionModel*> cms;

    float ringRadius;
    int numRings;
	int shrineID;

    void spawnChildren();
    void despawnChildren();

public:
	KDDRNGC();
	KDDRNGC(char data[32], bool useDefaultValues);

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
