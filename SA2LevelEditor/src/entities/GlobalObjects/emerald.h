#ifndef EMERALD_H
#define EMERALD_H

class TexturedModel;
class CollisionModel;
class Dummy;
class EMERALD_F;

#include <list>
#include "../entity.h"
#include "../sa2object.h"

class EMERALD : public SA2Object
{
private:
	static std::list<TexturedModel*> models;

    static CollisionModel* cmBase;

    Dummy* hitbox;

    short pieceID;
    short rotY;
    short rotZ;

    float var1;
    float radius;
    float var3;

public:
	EMERALD();
	EMERALD(char data[32], bool useDefaultValues);
    friend class EMERALD_F;

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
