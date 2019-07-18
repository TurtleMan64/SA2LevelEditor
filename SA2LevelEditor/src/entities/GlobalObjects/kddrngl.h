#ifndef KDDRNGL_H
#define KDDRNGL_H

class TexturedModel;
class CollisionModel;
class Dummy;

#include <list>
#include <vector>
#include "../entity.h"
#include "../sa2object.h"


class KDDRNGL : public SA2Object
{
private:
    std::vector<Dummy*> rings;
    std::vector<CollisionModel*> cms;

    int shrineID;
    float curveHeight;
    float ringDelta;
    int numRings;

    void spawnChildren();
    void despawnChildren();

public:
    KDDRNGL();
    KDDRNGL(char data[32], bool useDefaultValues);

    void updateEditorWindows();

    void updateValue(int btnIndex);

    void fillData(char data[32]);

    bool isSA2Object();

    void step();

    void cleanUp();

    std::list<TexturedModel*>* getModels();

    static void loadStaticModels();

    static void deleteStaticModels();
};
#endif
