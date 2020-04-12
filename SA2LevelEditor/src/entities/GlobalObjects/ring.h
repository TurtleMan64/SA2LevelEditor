#ifndef RING_H
#define RING_H

class TexturedModel;
class CollisionModel;

#include <list>
#include "../entity.h"
#include "../sa2object.h"

class RING : public SA2Object
{
private:
    static std::list<TexturedModel*> models;

    static CollisionModel* cmBase;

    friend class RING_LINEAR;
    friend class RING_CIRCLE;
    friend class SWDRNGL;
    friend class SWDRNGC;
    friend class KDDRNGL;
    friend class KDDRNGC;
    friend class SG_RING;

public:
    RING();
    RING(char data[32], bool useDefaultValues);

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
