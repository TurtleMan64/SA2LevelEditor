#ifndef TEXTUREDMODEL_H
#define TEXTUREDMODEL_H

#include <glad/glad.h>
#include <list>

#include "rawmodel.h"
#include "modeltexture.h"


class TexturedModel
{
private:
	RawModel rawModel;
	ModelTexture texture;

public:
	TexturedModel(RawModel*, ModelTexture*);

	TexturedModel();

	RawModel* getRawModel();

	ModelTexture* getTexture();

	void deleteMe();
};
#endif
