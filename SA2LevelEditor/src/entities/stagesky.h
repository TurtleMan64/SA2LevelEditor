#ifndef STAGESKY_H
#define STAGESKY_H

class TexturedModel;

#include <list>
#include "entity.h"

class StageSky : public Entity
{
private:
    static std::list<TexturedModel*> models;

public:
    StageSky();

    void step();

    std::list<TexturedModel*>* getModels();

    static void loadModels(const char* objFolder, const char* objFilename);

    static void deleteStaticModels();
};
#endif
