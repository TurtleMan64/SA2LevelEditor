#ifndef BUNCHIN_H
#define BUNCHIN_H

class TexturedModel;
class CollisionModel;
class Dummy;

#include <list>
#include <vector>
#include "../entity.h"
#include "../sa2object.h"

class BUNCHIN : public SA2Object
{
private:
    static std::list<TexturedModel*> models;

    static CollisionModel* cmBase;

    std::vector<Dummy*> guides;
    Dummy* topWeight = nullptr;

    int dropType;
    int switchID;
    float weightHeight;

    float scaleToUnits(float scale);
    float unitsToScale(float units);

    void spawnGuides();
    void despawnGuides();

public:
    BUNCHIN();
    BUNCHIN(char data[32], bool useDefaultValues);

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
