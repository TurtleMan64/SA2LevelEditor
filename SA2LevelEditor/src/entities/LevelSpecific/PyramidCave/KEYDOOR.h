#pragma once

class CollisionModel;

#include <list>
#include <vector>
#include "../../entity.h"
#include "../../sa2object.h"

class KEYDOOR : public SA2Object
{
private:
    static std::list<TexturedModel*> models;
    static CollisionModel* cmBase;
public:
    KEYDOOR();
    KEYDOOR(char data[32], bool useDefaultValues);

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

