#ifndef KDSPRING_H
#define KDSPRING_H

class TexturedModel;
class CollisionModel;
class Dummy;

#include <list>
#include <vector>
#include "../entity.h"
#include "../sa2object.h"

class KDSPRING : public SA2Object
{
private:
    std::vector<Dummy*> guides;

    int controlLockTime;
    float power;
    int shrineID;

    void spawnGuides();
    void despawnGuides();

public:
    KDSPRING();
    KDSPRING(char data[32], bool useDefaultValues);

    void step();

    void cleanUp();

    void updateEditorWindows();

    void updateValue(int btnIndex);

    void fillData(char data[32]);

    bool isSA2Object();

    std::list<TexturedModel*>* getModels();

    static void loadStaticModels();

    static void deleteStaticModels();
};
#endif
