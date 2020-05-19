#ifndef UNKNOWN_H
#define UNKNOWN_H

class TexturedModel;

#include <list>
#include <vector>
#include "entity.h"
#include "sa2object.h"
#include "collideableobject.h"

class Unknown : public SA2Object
{
private:
    float var1, var2, var3;

    static std::vector<std::list<TexturedModel*>> models;

    static CollisionModel* cmBase;

public:
    static std::list<TexturedModel*> modelsGuide;
    static std::list<TexturedModel*> modelsTriggerCube;
    static std::list<TexturedModel*> modelsTriggerSphere;
    static std::list<TexturedModel*> modelsTriggerCylinder;

    Unknown();
    Unknown(char data[32], bool useDefaultValues);

    void updateEditorWindows();

    void updateValue(int btnIndex);

    void fillData(char data[32]);

    bool isSA2Object();

    void step();

    void cleanUp();

    std::list<TexturedModel*>* getModels();

    static void loadStaticModels();

    static void deleteStaticModels();
};
#endif
