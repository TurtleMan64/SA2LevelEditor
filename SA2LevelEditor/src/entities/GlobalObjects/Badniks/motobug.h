#ifndef MOTOBUG_H
#define MOTOBUG_H

class TexturedModel;
class CollisionModel;

#include <list>
#include "../../entity.h"
#include "../../sa2object.h"

class MOTOBUG : public SA2Object
{
private:
    static std::list<TexturedModel*> models;

    static CollisionModel* cmBase;

    float var1;
    float var2;
    float var3;

public:
    MOTOBUG();
    MOTOBUG(char data[32], bool useDefaultValues);

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
