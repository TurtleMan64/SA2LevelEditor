#ifndef SPRB_H
#define SPRB_H

class TexturedModel;
class CollisionModel;

#include <list>
#include "../entity.h"
#include "../sa2object.h"

class SPRB : public SA2Object
{
private:
	static std::list<TexturedModel*> models;

    static CollisionModel* cmBase;

    int controlLockTime;
    float power;
    float var3;

public:
	SPRB();
	SPRB(char data[32]);

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
