#ifndef DASHRING_H
#define DASHRING_H

class TexturedModel;
class CollisionModel;
class Dummy;

#include <list>
#include <vector>
#include "../../entity.h"
#include "../../sa2object.h"

class DASHRING : public SA2Object
{
private:
    static std::list<TexturedModel*> modelsDash;
    static std::list<TexturedModel*> modelsRainbow;

    static CollisionModel* cmBaseDash;
    static CollisionModel* cmBaseRainbow;

    std::vector<Dummy*> guides;

    int controlLockTime;
    float power;
    
    char changeCamera;
    char rainbow;

    void spawnGuides();
    void despawnGuides();

public:
    DASHRING();
    DASHRING(char data[32], bool useDefaultValues);

    void step();

    void cleanUp();

    void updateEditorWindows();

    void updateValue(int btnIndex);

    void fillData(char data[32]);

    std::string toSabString();

    bool isSA2Object();

    std::list<TexturedModel*>* getModels();

    static void loadStaticModels();

    static void deleteStaticModels();
};
#endif
