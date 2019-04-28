#include <glad/glad.h>

#include "../loading/objLoader.h"
#include "entity.h"
#include "../models/texturedmodel.h"
#include "stagesky.h"
#include "../loading/loader.h"
#include "../main/main.h"
#include "camera.h"

#include <list>

std::list<TexturedModel*> StageSky::models;

StageSky::StageSky()
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
    baseColour.set(1.0f, 1.0f, 1.0f);
	
	updateTransformationMatrix();
}

void StageSky::step()
{
    position.set(&Global::gameCamera->eye);

    //some stages have ths sky not always be in the center of the camera.
    // example: green forest, metal harbor i think.
    // TODO deal with this
    //switch (Global::levelID)
    //{
    //    
    //}

    updateTransformationMatrix();
}

std::list<TexturedModel*>* StageSky::getModels()
{
	return &StageSky::models;
}

void StageSky::loadModels(const char* objFolder, const char* objFilename)
{
    if (StageSky::models.size() == 0)
	{
		#ifdef DEV_MODE
		std::fprintf(stdout, "Loading StageSky static models...\n");
		#endif

		loadModel(&StageSky::models, objFolder, objFilename);
	}
}

void StageSky::deleteStaticModels()
{
	#ifdef DEV_MODE
	std::fprintf(stdout, "Deleting StageSky static models...\n");
	#endif

	Entity::deleteModels(&StageSky::models);
}
