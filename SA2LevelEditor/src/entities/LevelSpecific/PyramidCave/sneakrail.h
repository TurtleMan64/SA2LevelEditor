#ifndef SNEAKRAIL_H
#define SNEAKRAIL_H

class TexturedModel;
class CollisionModel;

#include <list>
#include "../../entity.h"
#include "../../sa2object.h"

class SNEAKRAIL : public SA2Object
{
private:
    static std::list<TexturedModel*> models;

    static CollisionModel* cmBase;

    float var1 = 0.0f;
    float var2 = 0.0f;
    float var3 = 0.0f;

public:
    SNEAKRAIL();
    SNEAKRAIL(char data[32], bool useDefaultValues);

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
