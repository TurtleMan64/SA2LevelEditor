#ifndef SOAPSW_H
#define SOAPSW_H

class TexturedModel;
class CollisionModel;

#include <list>
#include "../entity.h"
#include "../sa2object.h"

class SOAP_SW : public SA2Object
{
private:
    static std::list<TexturedModel*> modelsSphere;
    static std::list<TexturedModel*> modelsCube;

    static CollisionModel* cmBaseSphere;
    static CollisionModel* cmBaseCube;
    
    bool isSphere;

    short noInputTime;
    short rotY;
    short rotZ;
    
    float var1;
    float var2;
    float var3;

public:
    SOAP_SW();
    SOAP_SW(char data[32], bool useDefaultValues);

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
