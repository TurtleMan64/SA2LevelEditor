#ifndef BIGJUMP_H
#define BIGJUMP_H

class TexturedModel;
class CollisionModel;
class Dummy;

#include <list>
#include <vector>
#include "../entity.h"
#include "../sa2object.h"

class BIGJUMP : public SA2Object
{
private:
    static std::list<TexturedModel*> models;

    static CollisionModel* cmBase;

    std::vector<Dummy*> guides;

    float powerH;
    int   cooldown; //?
    float powerV;

    void spawnGuides();
    void despawnGuides();

public:
    BIGJUMP();
    BIGJUMP(char data[32], bool useDefaultValues);

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
