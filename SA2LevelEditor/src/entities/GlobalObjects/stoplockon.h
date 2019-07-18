#ifndef STOPLOCKON_H
#define STOPLOCKON_H

class TexturedModel;
class CollisionModel;
class Dummy;

#include <list>
#include "../entity.h"
#include "../sa2object.h"

class STOPLOCKON : public SA2Object
{
private:
    static std::list<TexturedModel*> models;

    static CollisionModel* cmBase;

    Dummy* box = nullptr;

    float var1;
    float var2;
    float var3;

public:
    STOPLOCKON();
    STOPLOCKON(char data[32], bool useDefaultValues);

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
