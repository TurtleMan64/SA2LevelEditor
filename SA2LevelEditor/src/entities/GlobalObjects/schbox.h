#ifndef SCHBOX_H
#define SCHBOX_H

class TexturedModel;
class CollisionModel;

#include <list>
#include <vector>
#include "../entity.h"
#include "../sa2object.h"

class SCHBOX : public SA2Object
{
private:
    static std::list<TexturedModel*> models;

    static CollisionModel* cmBase;

    float var1;
    float var2;
    float var3;

public:
    SCHBOX();
    SCHBOX(char data[32], bool useDefaultValues);

    void step();

    void cleanUp();

    void updateEditorWindows();

    void updateValue(int btnIndex);

    void fillData(char data[32]);

    bool isSA2Object();

    std::string toSabString();

    std::list<TexturedModel*>* getModels();

    static void loadStaticModels();

    static void deleteStaticModels();
};
#endif
