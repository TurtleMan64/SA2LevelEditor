#ifndef UNKNOWN_H
#define UNKNOWN_H

class TexturedModel;

#include <list>
#include <vector>
#include "entity.h"
#include "sa2object.h"
#include "collideableobject.h"

class Unknown : public SA2Object
{
private:
    float var1, var2, var3;

	static std::vector<std::list<TexturedModel*>> models;

    static CollisionModel* cmBase;

public:
	Unknown();
	Unknown(char data[32]);

    void updateEditorWindows();

    void updateValue(int btnIndex);

    void fillData(char data[32]);

    bool isSA2Object();

	void step();

	std::list<TexturedModel*>* getModels();

	static void loadStaticModels();

	static void deleteStaticModels();
};
#endif
