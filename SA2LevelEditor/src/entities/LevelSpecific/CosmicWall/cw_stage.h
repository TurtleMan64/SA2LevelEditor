#ifndef CW_STAGE_H
#define CW_STAGE_H

class TexturedModel;
class CollisionModel;

#include <list>
#include "../../entity.h"
#include "../../sa2object.h"

class CW_STAGE : public SA2Object
{
private:
    static std::list<TexturedModel*> models;
    static std::list<TexturedModel*> modelsRails;

    static CollisionModel* cmBase;
    static CollisionModel* cmBaseRails;

    bool hasRails();

public:
    CW_STAGE();
    CW_STAGE(char data[32], bool useDefaultValues);

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
