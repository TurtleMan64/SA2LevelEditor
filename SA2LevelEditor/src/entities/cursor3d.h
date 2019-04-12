#ifndef CURSOR3D_H
#define CURSOR3D_H

class TexturedModel;
class GuiTexture;

#include <list>
#include "entity.h"

class Cursor3D : public Entity
{
public:
    static GuiTexture* textureCursor3D;

	Cursor3D();

	void step();

	std::list<TexturedModel*>* getModels();

	static void loadStaticModels();

	static void deleteStaticModels();
};
#endif
