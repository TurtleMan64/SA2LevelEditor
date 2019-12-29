#ifndef SPIDERWEB_H
#define SPIDERWEB_H

class TexturedModel;
class CollisionModel;
class Dummy;

#include <list>
#include "../../entity.h"
#include "../../sa2object.h"

class SPIDERWEB : public SA2Object
{
private:
    static std::list<TexturedModel*> models;

    static CollisionModel* cmBase;

    Dummy* web = nullptr;

public:
    SPIDERWEB();
    SPIDERWEB(char data[32], bool useDefaultValues);

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
