#ifdef _WIN32
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>

#include <string>
#include <cstring>
#include <unordered_map>
#include <unordered_set>
#include <list>

#include <ctime>
#include <random>

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#endif

#include "main.h"
#include "displaymanager.h"
#include "../toolbox/input.h"
#include "../models/rawmodel.h"
//#include "../shaders/shaderprogram.h"
#include "../models/modeltexture.h"
#include "../entities/entity.h"
#include "../toolbox/vector.h"
#include "../toolbox/matrix.h"
#include "../entities/camera.h"
//#include "../objLoader/objLoader.h"
#include "../entities/stage.h"
#include "../loading/levelloader.h"
#include "../collision/collisionchecker.h"
//#include "../entities/skysphere.h"
//#include "../guis/guimanager.h"
//#include "../particles/particlemaster.h"
//#include "../particles/particleresources.h"
#include "../toolbox/split.h"
//#include "../guis/guirenderer.h"
//#include "../guis/guitextureresources.h"
#include "../toolbox/level.h"
//#include "../guis/guitexture.h"
//#include "../particles/particle.h"
//#include "../entities/skysphere.h"
#include "../toolbox/getline.h"
#include "../rendering/masterrenderer.h"

#ifdef _WIN32
#include <windows.h>
#include <tchar.h>
#endif


std::unordered_set<Entity*> Global::gameEntities;
std::list<Entity*> Global::gameEntitiesToAdd;
std::list<Entity*> Global::gameEntitiesToDelete;

std::unordered_set<Entity*> Global::gameEntitiesPass2;
std::list<Entity*> Global::gameEntitiesPass2ToAdd;
std::list<Entity*> Global::gameEntitiesPass2ToDelete;

std::unordered_set<Entity*> Global::gameEntitiesPass3;
std::list<Entity*> Global::gameEntitiesPass3ToAdd;
std::list<Entity*> Global::gameEntitiesPass3ToDelete;

std::unordered_set<Entity*> Global::gameTransparentEntities;
std::list<Entity*> Global::gameTransparentEntitiesToAdd;
std::list<Entity*> Global::gameTransparentEntitiesToDelete;


float  dt = 0;
double timeOld = 0;
double timeNew = 0;
bool Global::refreshWindow = true;
Camera*    Global::gameCamera    = nullptr;
Stage*     Global::gameStage     = nullptr;
SkySphere* Global::gameSkySphere = nullptr;

std::string Global::dirSA2Root = "C:/Program Files (x86)/Steam/steamapps/common/Sonic Adventure 2";

extern unsigned int SCR_WIDTH;
extern unsigned int SCR_HEIGHT;

int Global::countNew = 0;
int Global::countDelete = 0;
int Global::gameState = 0;
int Global::levelID = 0;

int Global::gameMissionNumber = 0;
std::unordered_map<std::string, std::string> Global::levelFileMap;

int main()
{
    return Global::main();
}

int Global::main()
{
	Global::countNew = 0;
	Global::countDelete = 0;

	srand(0);

	DisplayManager::createDisplay();

	#if !defined(DEV_MODE) && defined(_WIN32)
	FreeConsole();
	#endif

	Input::init();

	//This camera is never deleted.
	Camera cam;
	Global::gameCamera = &cam;

	MasterRenderer::init();

	LevelLoader::loadLevelData();

	//Global::fontVipnagorgialla = new FontType(Loader::loadTexture("res/Fonts/vipnagorgialla.png"), "res/Fonts/vipnagorgialla.fnt"); INCR_NEW("FontType");

	//TextMaster::init();

	//GuiManager::init();


	//if (Global::renderParticles)
	{
		//ParticleResources::loadParticles();
	}

	//GuiTextureResources::loadGuiTextures();

	CollisionChecker::initChecker();

	//This stage never gets deleted.
	Stage stage;
	Global::gameStage = &stage;

	//This sky sphere never gets deleted.
	//SkySphere skySphere;
	//Global::gameSkySphere = &skySphere;

	//ParticleMaster::init(Master_getProjectionMatrix());


	glfwSetTime(0);

	int frameCount = 0;
	double previousTime = 0;

    LevelLoader::loadLevel("Metal Harbor");

	Global::gameState = STATE_RUNNING;

	while (Global::gameState != STATE_EXITING && DisplayManager::displayWantsToClose() == 0)
	{
		frameCount++;
		timeNew = glfwGetTime();
		dt = (float)(timeNew - timeOld);
		dt = std::fminf(dt, 0.04f); //Anything lower than 25fps will slow the gameplay down

		timeOld = timeNew;

		Input::waitForInputs();

        if (!Global::refreshWindow)
        {
            continue;
        }
        Global::refreshWindow = false;

		GLenum err = glGetError();
		if (err != GL_NO_ERROR)
		{
			std::fprintf(stderr, "########  GL ERROR  ########\n");
			std::fprintf(stderr, "%d\n", err);
		}

		//long double thisTime = std::time(0);
		//std::fprintf(stdout, "time: %f time\n", thisTime);

		//entities managment
		for (auto entityToAdd : gameEntitiesToAdd)
		{
			gameEntities.insert(entityToAdd);
		}
		gameEntitiesToAdd.clear();

		for (auto entityToDelete : gameEntitiesToDelete)
		{
			gameEntities.erase(entityToDelete);
			delete entityToDelete; INCR_DEL("Entity");
		}
		gameEntitiesToDelete.clear();


		//entities pass2 managment
		for (auto entityToAdd : gameEntitiesPass2ToAdd)
		{
			gameEntitiesPass2.insert(entityToAdd);
		}
		gameEntitiesPass2ToAdd.clear();

		for (auto entityToDelete : gameEntitiesPass2ToDelete)
		{
			gameEntitiesPass2.erase(entityToDelete);
			delete entityToDelete; INCR_DEL("Entity");
		}
		gameEntitiesPass2ToDelete.clear();


		//entities pass3 managment
		for (auto entityToAdd : gameEntitiesPass3ToAdd)
		{
			gameEntitiesPass3.insert(entityToAdd);
		}
		gameEntitiesPass3ToAdd.clear();

		for (auto entityToDelete : gameEntitiesPass3ToDelete)
		{
			gameEntitiesPass3.erase(entityToDelete);
			delete entityToDelete; INCR_DEL("Entity");
		}
		gameEntitiesPass3ToDelete.clear();


		//transnaprent entities managment
		for (auto entityToAdd : gameTransparentEntitiesToAdd)
		{
			gameTransparentEntities.insert(entityToAdd);
		}
		gameTransparentEntitiesToAdd.clear();

		for (auto entityToDelete : gameTransparentEntitiesToDelete)
		{
			gameTransparentEntities.erase(entityToDelete);
			delete entityToDelete; INCR_DEL("Entity");
		}
		gameTransparentEntitiesToDelete.clear();

		switch (Global::gameState)
		{
			case STATE_RUNNING:
			{
				//game logic

				for (Entity* e : gameEntities)
				{
					e->step();
				}
				for (Entity* e : gameEntitiesPass2)
				{
					e->step();
				}
				for (Entity* e : gameEntitiesPass3)
				{
					e->step();
				}
				for (Entity* e : gameTransparentEntities)
				{
					e->step();
				}
				//skySphere.step();
                //ModelTexture::updateAnimations(dt);
				Global::gameCamera->refresh();
				//if (Global::renderParticles)
				{
					//ParticleMaster::update(Global::gameCamera);
				}
				//Global::gameClock+=dt;
				break;
			}

			default:
				break;
		}

		//prepare entities to render
		for (Entity* e : gameEntities)
		{
			MasterRenderer::processEntity(e);
		}
		for (Entity* e : gameEntitiesPass2)
		{
			MasterRenderer::processEntityPass2(e);
		}
		for (Entity* e : gameEntitiesPass3)
		{
			MasterRenderer::processEntityPass3(e);
		}
		for (Entity* e : gameTransparentEntities)
		{
			MasterRenderer::processTransparentEntity(e);
		}
		
		MasterRenderer::processEntity(&stage);
		//MasterRenderer::processEntity(&skySphere);

		glEnable(GL_CLIP_DISTANCE1);
		MasterRenderer::render(&cam);
		glDisable(GL_CLIP_DISTANCE1);

		//if (Global::renderParticles)
		//{
			//ParticleMaster::renderParticles(&cam, SkyManager::getOverallBrightness(), 0);
		//}

		MasterRenderer::clearEntities();
		MasterRenderer::clearEntitiesPass2();
		MasterRenderer::clearEntitiesPass3();
		MasterRenderer::clearTransparentEntities();

		//GuiManager::refresh();
		//TextMaster::render();

		DisplayManager::updateDisplay();

		if (timeNew - previousTime >= 1.0)
		{
			//std::fprintf(stdout, "fps: %f\n", frameCount / (timeNew - previousTime));
			//std::fprintf(stdout, "diff: %d\n", Global::countNew - Global::countDelete);
			//Loader::printInfo();
			//std::fprintf(stdout, "entity counts: %d %d %d\n", gameEntities.size(), gameEntitiesPass2.size(), gameTransparentEntities.size());
			frameCount = 0;
			previousTime = timeNew;
		}

		//std::fprintf(stdout, "dt: %f\n", dt);
		//std::this_thread::sleep_for(std::chrono::milliseconds(8));
	}

	MasterRenderer::cleanUp();
	//Loader::cleanUp();
	//TextMaster::cleanUp();
	//GuiRenderer::cleanUp();
	DisplayManager::closeDisplay();

	return 0;
}

//The newEntity should be created with the new keyword, as it will be deleted later
void Global::addEntity(Entity* entityToAdd)
{
	gameEntitiesToAdd.push_back(entityToAdd);
}

void Global::deleteEntity(Entity* entityToDelete)
{
	gameEntitiesToDelete.push_back(entityToDelete);
}

void Global::deleteAllEntites()
{
	//Make sure no entities get left behind in transition
	for (Entity* entityToAdd : gameEntitiesToAdd)
	{
		gameEntities.insert(entityToAdd);
	}
	gameEntitiesToAdd.clear();

	for (Entity* entityToDelete : gameEntitiesToDelete)
	{
		gameEntities.erase(entityToDelete);
		delete entityToDelete; INCR_DEL("Entity");
	}
	gameEntitiesToDelete.clear();


	//Delete all the rest
	for (Entity* entityToDelete : gameEntities)
	{
		delete entityToDelete; INCR_DEL("Entity");
	}
	gameEntities.clear();
}

void Global::addEntityPass2(Entity* entityToAdd)
{
	gameEntitiesPass2ToAdd.push_back(entityToAdd);
}

void Global::deleteEntityPass2(Entity* entityToDelete)
{
	gameEntitiesPass2ToDelete.push_back(entityToDelete);
}

void Global::deleteAllEntitesPass2()
{
	//Make sure no entities get left behind in transition
	for (Entity* entityToAdd : gameEntitiesPass2ToAdd)
	{
		gameEntitiesPass2.insert(entityToAdd);
	}
	gameEntitiesPass2ToAdd.clear();

	for (Entity* entityToDelete : gameEntitiesPass2ToDelete)
	{
		gameEntitiesPass2.erase(entityToDelete);
		delete entityToDelete; INCR_DEL("Entity");
	}
	gameEntitiesPass2ToDelete.clear();

	for (Entity* entityToDelete : gameEntitiesPass2)
	{
		delete entityToDelete; INCR_DEL("Entity");
	}
	gameEntitiesPass2.clear();
}

void Global::addEntityPass3(Entity* entityToAdd)
{
	gameEntitiesPass3ToAdd.push_back(entityToAdd);
}

void Global::deleteEntityPass3(Entity* entityToDelete)
{
	gameEntitiesPass3ToDelete.push_back(entityToDelete);
}

void Global::deleteAllEntitesPass3()
{
	//Make sure no entities get left behind in transition
	for (Entity* entityToAdd : gameEntitiesPass3ToAdd)
	{
		gameEntitiesPass3.insert(entityToAdd);
	}
	gameEntitiesPass3ToAdd.clear();

	for (Entity* entityToDelete : gameEntitiesPass3ToDelete)
	{
		gameEntitiesPass3.erase(entityToDelete);
		delete entityToDelete; INCR_DEL("Entity");
	}
	gameEntitiesPass3ToDelete.clear();

	for (Entity* entityToDelete : gameEntitiesPass3)
	{
		delete entityToDelete; INCR_DEL("Entity");
	}
	gameEntitiesPass3.clear();
}

//Transparent entities shouldn't create new transparent entities from within their step function
void Global::addTransparentEntity(Entity* entityToAdd)
{
	gameTransparentEntities.insert(entityToAdd);
}

void Global::deleteTransparentEntity(Entity* entityToDelete)
{
	gameTransparentEntities.erase(entityToDelete);
	delete entityToDelete; INCR_DEL("Entity");
}

void Global::deleteAllTransparentEntites()
{
	for (Entity* entityToDelete : gameTransparentEntities)
	{
		delete entityToDelete; INCR_DEL("Entity");
	}
	gameTransparentEntities.clear();
}

std::unordered_map<std::string, int> heapObjects;

void Global::debugNew(const char* name)
{
    Global::countNew++;
    name;

    #ifdef DEV_MODE
    if (heapObjects.find(name) == heapObjects.end())
    {
        heapObjects[name] = 1;
    }
    else
    {
        int num = heapObjects[name];
        heapObjects[name] = num+1;
    }
    #endif
}

void Global::debugDel(const char* name)
{
    Global::countDelete++;
    name;

    #ifdef DEV_MODE
    if (heapObjects.find(name) == heapObjects.end())
    {
        std::fprintf(stdout, "Warning: trying to delete '%s' when there are none.\n", name);
        heapObjects[name] = 0;
    }
    else
    {
        int num = heapObjects[name];
        heapObjects[name] = num-1;
    }
    #endif
}
