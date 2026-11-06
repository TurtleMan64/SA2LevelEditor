#ifndef STAGECOLLISION_H
#define STAGECOLLISION_H

class TexturedModel;
class CollisionModel;

#include <list>
#include "entity.h"
#include "sa2object.h"

class StageCollision : public SA2Object
{
private:
    static std::list<TexturedModel*> models;
    static CollisionModel* collisionModel;

public:
    StageCollision();

    void step();

    std::list<TexturedModel*>* getModels();

    static void loadModels(const char* objFolder, const char* objFilename);

	void updateEditorWindows();

    static void deleteStaticModels();
};
#endif
