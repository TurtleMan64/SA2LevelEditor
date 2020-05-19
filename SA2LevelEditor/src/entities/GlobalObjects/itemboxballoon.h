#ifndef ITEMBOXBALLOON_H
#define ITEMBOXBALLOON_H

class TexturedModel;
class CollisionModel;

#include <list>
#include <vector>
#include "../entity.h"
#include "../sa2object.h"

class ITEMBOXBALLOON : public SA2Object
{
private:
    static std::list<TexturedModel*> models;

    static CollisionModel* cmBase;

    int itemType;
    float var2;
    float var3;

public:
    ITEMBOXBALLOON();
    ITEMBOXBALLOON(char data[32], bool useDefaultValues);

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
