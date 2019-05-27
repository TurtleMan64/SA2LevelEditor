#ifndef ROCKET_H
#define ROCKET_H

class TexturedModel;
class CollisionModel;
class Dummy;

#include <list>
#include <vector>
#include "../entity.h"
#include "../sa2object.h"

class ROCKET : public SA2Object
{
private:
	static std::list<TexturedModel*> models;

    static CollisionModel* cmBase;

    std::vector<Dummy*> guides;
    
    bool needsSwitch;
    int ogRotY;
    int switchID;
    float endX;
    float endY;
    float endZ;

    //based on where the endpoint is
    void calculateRotY();

    void spawnGuides();
    void despawnGuides();

public:
	ROCKET();
	ROCKET(char data[32], bool useDefaultValues);

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
