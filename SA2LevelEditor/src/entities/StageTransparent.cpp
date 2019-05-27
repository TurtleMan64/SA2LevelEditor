#include <glad/glad.h>

#include "../loading/objLoader.h"
#include "entity.h"
#include "../models/texturedmodel.h"
#include "stagetransparent.h"
#include "../loading/loader.h"
#include "../main/main.h"

#include <list>

std::list<TexturedModel*> StageTransparent::models;

StageTransparent::StageTransparent()
{
	
}

StageTransparent::StageTransparent(const char* objFolder, const char* objFilename)
{
	position.x = 0;
	position.y = 0;
	position.z = 0;
	rotationX = 0;
	rotationY = 0;
	rotationZ = 0;
	scaleX = 1;
    scaleY = 1;
    scaleZ = 1;
	visible = true;
	
	if (StageTransparent::models.size() == 0)
	{
		#ifdef DEV_MODE
		std::fprintf(stdout, "Loading StageTransparent static models...\n");
		#endif

		loadModel(&StageTransparent::models, objFolder, objFilename);
	}
	
	updateTransformationMatrixYXZ();
}

void StageTransparent::step()
{

}

std::list<TexturedModel*>* StageTransparent::getModels()
{
	return &StageTransparent::models;
}

void StageTransparent::deleteStaticModels()
{
	#ifdef DEV_MODE
	std::fprintf(stdout, "Deleting StageTransparent static models...\n");
	#endif

	Entity::deleteModels(&StageTransparent::models);
}
