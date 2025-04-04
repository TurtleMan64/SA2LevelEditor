#ifndef ITEMBOX_H
#define ITEMBOX_H

class TexturedModel;
class CollisionModel;
class Dummy;

#include <list>
#include <vector>
#include "../entity.h"
#include "../sa2object.h"

class ITEMBOX : public SA2Object
{
private:
    static std::list<TexturedModel*> modelsBase;
    static std::list<TexturedModel*> modelsShell;
    static std::vector<std::list<TexturedModel*>> modelsItem;

    static CollisionModel* cmBaseBase;
    static CollisionModel* cmBaseShell;

    Dummy* shell = nullptr;
    Dummy* item = nullptr;

    CollisionModel* collideModelShellTransformed = nullptr;

    int itemType;
    float var2;
    float var3;

    static void loadNewItemModel(std::string folder, std::string filename);

public:
    ITEMBOX();
    ITEMBOX(char data[32], bool useDefaultValues);

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
