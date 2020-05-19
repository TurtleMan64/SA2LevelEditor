#ifndef KASOKU_H
#define KASOKU_H

class TexturedModel;
class CollisionModel;

#include <list>
#include "../entity.h"
#include "../sa2object.h"

class KASOKU : public SA2Object
{
private:
    static std::list<TexturedModel*> models;

    static CollisionModel* cmBase;

    float power;
    int   cooldown;
    float var3;

public:
    KASOKU();
    KASOKU(char data[32], bool useDefaultValues);

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
