#ifndef TJUMPDAI_H
#define TJUMPDAI_H

class TexturedModel;
class CollisionModel;
class Dummy;

#include <list>
#include <vector>
#include "../entity.h"
#include "../sa2object.h"

class TJUMPDAI : public SA2Object
{
private:
    static std::list<TexturedModel*> models;

    static CollisionModel* cmBase;

    float var1 = 0.0f;
    float var2 = 0.0f;
    float var3 = 0.0f;

public:
    TJUMPDAI();
    TJUMPDAI(char data[32], bool useDefaultValues);

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
