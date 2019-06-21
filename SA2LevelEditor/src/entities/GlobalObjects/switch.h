#ifndef SWITCH_H
#define SWITCH_H

class TexturedModel;
class CollisionModel;
class Dummy;

#include <list>
#include <vector>
#include "../entity.h"
#include "../sa2object.h"

class SWITCH : public SA2Object
{
private:
	static std::list<TexturedModel*> models;

	static CollisionModel* cmBase;

	std::vector<SA2Object*> linkedObjects;

	int type;
	int triggerID;
	float activeFrames;

	void updateID();
	void highlight();
	void unhighlight();

public:
	SWITCH();
	SWITCH(char data[32], bool useDefaultValues);

	void step();

	void updateEditorWindows();

	void updateValue(int btnIndex);

	void fillData(char data[32]);

	int getSwitchID();

	bool isSA2Object();

	std::list<TexturedModel*>* getModels();

	static void loadStaticModels();

	static void deleteStaticModels();
};
#endif