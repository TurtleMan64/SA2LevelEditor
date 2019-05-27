#ifndef LINKLINK_H
#define LINKLINK_H

class TexturedModel;
class CollisionModel;

#include <list>
#include "../entity.h"
#include "../sa2object.h"

class LINKLINK : public SA2Object
{
private:
	static std::list<TexturedModel*> modelsSphere;
    static std::list<TexturedModel*> modelsCube;

    static CollisionModel* cmBaseSphere;
    static CollisionModel* cmBaseCube;
    
    bool isSphere;

    short timer;
    short rotY;
    short rotZ;
    
    float var1;
    float var2;
    float var3;

public:
	LINKLINK();
	LINKLINK(char data[32], bool useDefaultValues);

	void step();

    void updateEditorWindows();

    void updateValue(int btnIndex);

    void fillData(char data[32]);

    bool isSA2Object();

	std::list<TexturedModel*>* getModels();

	static void loadStaticModels();

	static void deleteStaticModels();
};
#endif
