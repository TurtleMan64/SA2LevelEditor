#ifndef SNAKESTATUE_H
#define SNAKESTATUE_H

class TexturedModel;
class CollisionModel;

#include <list>
#include "../../entity.h"
#include "../../sa2object.h"

class SNAKESTATUE : public SA2Object
{
private:
    static std::list<TexturedModel*> modelsLeft;
    static std::list<TexturedModel*> modelsRight;
    static std::list<TexturedModel*> modelsLeftCollision;
    static std::list<TexturedModel*> modelsRightCollision;

    static CollisionModel* cmBaseLeft;
    static CollisionModel* cmBaseRight;
    static CollisionModel* cmBaseLeftCollision;
    static CollisionModel* cmBaseRightCollision;

    bool isLeft = false;

public:
    SNAKESTATUE();
    SNAKESTATUE(char data[32], bool useDefaultValues);

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
