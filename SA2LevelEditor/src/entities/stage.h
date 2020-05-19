#ifndef STAGE_H
#define STAGE_H

class TexturedModel;
class CollisionModel;

#include <list>
#include "entity.h"

class Stage : public Entity
{
private:
    static std::list<TexturedModel*> models;
    static CollisionModel* collisionModel;

public:
    Stage();

    void step();

    std::list<TexturedModel*>* getModels();

    static void loadModels(const char* objFolder, const char* objFilename);

    static void deleteStaticModels();
};
#endif
