#ifndef SGRING_H
#define SGRING_H

class TexturedModel;
class CollisionModel;

#include <list>
#include "../entity.h"
#include "../sa2object.h"

class SG_RING : public SA2Object
{

public:
	SG_RING();
	SG_RING(char data[32], bool useDefaultValues);

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
