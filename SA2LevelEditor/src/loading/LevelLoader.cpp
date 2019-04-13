#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <list>

#include <chrono>
#include <thread>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "levelloader.h"
#include "../main/main.h"
#include "../entities/stage.h"
#include "../entities/stagepass2.h"
#include "../entities/stagepass3.h"
#include "../entities/stagetransparent.h"
#include "../collision/collisionchecker.h"
#include "../collision/collisionmodel.h"
#include "objLoader.h"
#include "../toolbox/getline.h"
#include "../toolbox/split.h"
#include "../toolbox/input.h"
#include "../entities/camera.h"
#include "../entities/GlobalObjects/ring.h"
#include "../toolbox/readbytes.h"
#include "../entities/unknown.h"
#include "loader.h"
#include "../entities/stagecollision.h"

int LevelLoader::numLevels = 0;

void LevelLoader::loadTitle()
{
	Stage::deleteStaticModels();
	StagePass2::deleteStaticModels();
	StagePass3::deleteStaticModels();
	StageTransparent::deleteStaticModels();

	freeAllStaticModels();

	CollisionChecker::deleteAllCollideModels();

	Global::deleteAllEntites();
	Global::deleteAllEntitesPass2();
	Global::deleteAllEntitesPass3();
	Global::deleteAllTransparentEntites();

	
	//GuiManager::clearGuisToRender();
}

void LevelLoader::loadLevel(std::string levelName, int type)
{
    std::string fname = "";

    if (type == 0)
    {
        if (Global::levelFileMap.find(levelName) == Global::levelFileMap.end())
        {
            std::fprintf(stderr, "Trying to load a non existing level.\n");
            return;
        }

	    fname = "res/Levels/"+Global::levelFileMap[levelName];
    }
    else
    {
        fname = levelName;
    }

	freeAllStaticModels();

	Global::deleteAllEntites();
	Global::deleteAllEntitesPass2();
	Global::deleteAllEntitesPass3();
	Global::deleteAllTransparentEntites();

	Stage::deleteStaticModels();
	StagePass2::deleteStaticModels();
	StagePass3::deleteStaticModels();
	StageTransparent::deleteStaticModels();

    StageCollision::deleteStaticModels();

    CollisionChecker::deleteAllCollideModels();

    Global::gameCamera->reset();

	std::ifstream file(fname);
	if (!file.is_open())
	{
		std::fprintf(stdout, "Error: Cannot load file '%s'\n", (fname).c_str());
		file.close();
		return;
	}


	//Run through the header content

    std::string levelnum;
	getlineSafe(file, levelnum);
    int level = std::stoi(levelnum);
    Global::levelID = level;

    //setfile names
    std::string setS;
	getlineSafe(file, setS);
    std::string setU;
	getlineSafe(file, setU);
    std::string setHardS;
	getlineSafe(file, setHardS);
    std::string setHardU;
	getlineSafe(file, setHardU);

    //visible model directory
	std::string modelDir;
	getlineSafe(file, modelDir);
    //visible model name
    std::string modelName;
	getlineSafe(file, modelName);

	Stage::loadModels(modelDir.c_str(), modelName.c_str());



	//collision model directory
	std::string colDir;
	getlineSafe(file, colDir);
    //collision model name
    std::string colName;
	getlineSafe(file, colName);

    //load the new collision model
    StageCollision::loadModels(colDir.c_str(), colName.c_str());


    // load all the models associated with a specific level
    switch (Global::levelID)
    {
        case Global::Levels::Green_Forest:
            //Ring::loadStaticModels();
            //Unknown::loadStaticModels();
            break;

        case Global::Levels::Metal_Harbor:
            //Ring::loadStaticModels();
            //Unknown::loadStaticModels();
            break;

        default:
            break;
    }

	//Now read through all the objects defined in the file

	std::string line;

	while (!file.eof())
	{
		getlineSafe(file, line);

		char lineBuf[512]; //Buffer to copy line into
		memcpy(lineBuf, line.c_str(), line.size()+1);

		int splitLength = 0;
		char** lineSplit = split(lineBuf, ' ', &splitLength);

		if (splitLength > 0)
		{
			//Input::pollInputs();

			processLine(lineSplit, splitLength);
		}
		free(lineSplit);
	}
	file.close();


    //now run through the binary set files:
    std::string filepathS = (Global::dirSA2Root+"/resource/gd_PC/")+setS+".bin";
    std::vector<char> objectsSetS = readAllBytes(filepathS.c_str());
    for (int i = 0; i < (int)objectsSetS.size(); i+=32)
    {
        processObjectSET(&objectsSetS[i]);
    }

    //now run through the binary set files:
    std::string filepathU = (Global::dirSA2Root+"/resource/gd_PC/")+setU+".bin";
    std::vector<char> objectsSetU = readAllBytes(filepathU.c_str());
    for (int i = 0; i < (int)objectsSetU.size(); i+=32)
    {
        processObjectSET(&objectsSetU[i]);
    }

    //Loader::printInfo();

	Global::gameState = STATE_RUNNING;

	glfwSetTime(0);
	extern double timeOld;
	//extern double previousTime;
	timeOld = 0.0;
	//previousTime = 0.0;
}


void LevelLoader::processLine(char** dat, int /*datLength*/)
{
	if (dat[0][0] == '#')
	{
		return;
	}

	int id = std::stoi(dat[0]);

	switch (id)
	{
		case 2: //Stage Pass 2
		{
			StagePass2* pass2 = new StagePass2(dat[1], dat[2]); INCR_NEW("Entity");
			Global::addEntityPass2(pass2);
			return;
		}

		case 3: //Stage Pass 3
		{
			StagePass3* pass3 = new StagePass3(dat[1], dat[2]); INCR_NEW("Entity");
			Global::addEntityPass3(pass3);
			return;
		}

		case 4: //Stage Transparent
		{
			StageTransparent* trans = new StageTransparent(dat[1], dat[2]); INCR_NEW("Entity");
			Global::addTransparentEntity(trans);
			return;
		}

		default:
		{
			return;
		}
	}
}

void LevelLoader::processObjectSET(char data[32])
{
    int objectType = 0;
    objectType += data[1];//data[1]+(data[0]<<8);
    if (objectType >= 100 || objectType < 0)
    {
        std::fprintf(stdout, "objectType = %d\n", objectType);
    }

    switch (Global::levelID)
    {
        case Global::Levels::Green_Forest:
            switch (objectType)
            {
                case 0:
                    Global::addEntity(new Ring(data)); INCR_NEW("Entity")
                    break;

                default:
                    Global::addEntity(new Unknown(data, objectType)); INCR_NEW("Entity")
                    break;
            }
            break;

        case Global::Levels::Metal_Harbor:
            switch (objectType)
            {
                case 0:
                    Global::addEntity(new Ring(data)); INCR_NEW("Entity")
                    break;

                default:
                    Global::addEntity(new Unknown(data, objectType)); INCR_NEW("Entity")
                    break;
            }
            break;

        default: break;
    }

}

void LevelLoader::loadLevelData()
{
    Global::levelFileMap.clear();
    Global::levelFileMap["Green Forest"] = "GreenForest.lvl2";
    Global::levelFileMap["Metal Harbor"] = "MetalHarbor.lvl2";
}

float LevelLoader::toFloat(char* input)
{
	return std::stof(input);
}

int LevelLoader::toInt(char* input)
{
	return std::stoi(input);
}

void LevelLoader::freeAllStaticModels()
{
    //Ring::deleteStaticModels(); //dont delete these since theyll be in basically all levels
    //Unknown::deleteStaticModels();
}

int LevelLoader::getNumLevels()
{
	return LevelLoader::numLevels;
}
