#ifndef IRONBALL2_H
#define IRONBALL2_H

class TexturedModel;
class CollisionModel;

#include <list>
#include <tuple>
#include "../entity.h"
#include "../sa2object.h"
#include "../dummy.h"
#include "../unknown.h"

class IRONBALL2 : public SA2Object
{
private:
	static std::list<TexturedModel*> baseModels;
	static std::list<TexturedModel*> rodModels;
	static std::list<TexturedModel*> ballModels;

    static CollisionModel* cmBase;
	static CollisionModel* cmRod;
	static CollisionModel* cmBall;

	std::tuple<Dummy*, CollisionModel*>* rod;
	std::tuple<Dummy*, CollisionModel*>* ball1;
	std::tuple<Dummy*, CollisionModel*>* ball2;

	std::vector<Dummy*> guides;
	Dummy* swingCylinder;

    float radius;
    float rotSpeed;
	bool oscillates;
	float oscillation;

	int rotZ; //Stores the numerical value of z-rotation since it is interpreted as a boolean

	void createModel();
	void destroyModel();

	void spawnGuides();
	void despawnGuides();

public:
	IRONBALL2();
	IRONBALL2(char data[32], bool useDefaultValues);

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
