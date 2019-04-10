#include <glad/glad.h>

#include "../loading/objLoader.h"
#include "entity.h"
#include "../models/texturedmodel.h"
#include "stagepass3.h"
#include "../loading/loader.h"
#include "../main/main.h"

#include <list>

std::list<TexturedModel*> StagePass3::models;

StagePass3::StagePass3()
{
	
}

StagePass3::StagePass3(const char* objFolder, const char* objFilename)
{
	position.x = 0;
	position.y = 0;
	position.z = 0;
	rotationX = 0;
	rotationY = 0;
	rotationZ = 0;
	scale = 1;
	visible = true;
	
	if (StagePass3::models.size() == 0)
	{
		#ifdef DEV_MODE
		std::fprintf(stdout, "Loading StagePass3 static models...\n");
		#endif

		loadModel(&StagePass3::models, objFolder, objFilename);
	}
	
	updateTransformationMatrix();
}

void StagePass3::step()
{

}

std::list<TexturedModel*>* StagePass3::getModels()
{
	return &StagePass3::models;
}

void StagePass3::deleteStaticModels()
{
	#ifdef DEV_MODE
	std::fprintf(stdout, "Deleting StagePass3 static models...\n");
	#endif

	Entity::deleteModels(&StagePass3::models);
}
