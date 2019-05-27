#ifndef LIGHT_SW_H
#define LIGHT_SW_H

class TexturedModel;
class CollisionModel;
class Dummy;

#include <list>
#include "../entity.h"
#include "../sa2object.h"

class LIGHT_SW : public SA2Object
{
private:
    static std::list<TexturedModel*> modelsCube;
    static std::list<TexturedModel*> modelsSphere;
    static std::list<TexturedModel*> modelsCylinder;

    static CollisionModel* cmBaseCube;
    static CollisionModel* cmBaseSphere;
    static CollisionModel* cmBaseCylinder;

    Dummy* box;

    //0 = box zy
    //1 = sphere of mag var123
    //2 = cylinder yxz, radius var1, height var2
    int collisionType;

    float var1;
    float var2;
    float var3;

    void updateBoxTransformationMatrix();
    void updateMyCollisionModel();
    std::list<TexturedModel*>* getSpecificModels(int type);

public:
	LIGHT_SW();
	LIGHT_SW(char data[32], bool useDefaultValues);

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
