#ifndef UNKNOWN_H
#define UNKNOWN_H

class TexturedModel;

#include <list>
#include <vector>
#include "entity.h"
#include "sa2object.h"

class Unknown : public SA2Object
{
private:
    int id;

	static std::vector<std::list<TexturedModel*>> models;

public:
	Unknown();
	Unknown(char data[32], int id);

	void step();

	std::list<TexturedModel*>* getModels();

	static void loadStaticModels();

	static void deleteStaticModels();
};
#endif
