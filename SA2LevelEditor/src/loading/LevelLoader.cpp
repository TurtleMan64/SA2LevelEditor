#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <list>

#include <chrono>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "levelloader.h"
#include "../entities/unknown.h"
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
#include "../entities/GlobalObjects/ringlinear.h"
#include "../toolbox/readbytes.h"
#include "loader.h"
#include "../entities/stagecollision.h"
#include "../entities/stagekillplanes.h"

#ifdef _WIN32
#include <windows.h>
#include <commdlg.h>
#include <tchar.h>
#endif

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

void LevelLoader::loadLevel(int levelID, std::string setDir)
{
    std::string fname = "";

    if (Global::levelIDMap.find(levelID) == Global::levelIDMap.end())
    {
        std::fprintf(stderr, "Trying to load a non existing level.\n");
        return;
    }

	fname = "res/Levels/"+Global::levelIDMap[levelID];

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
    StageKillplanes::deleteStaticModels();

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


    //killplane directory
    std::string killDir;
	getlineSafe(file, killDir);
    //killplane model name
    std::string killName;
	getlineSafe(file, killName);
    StageKillplanes::loadModels(killDir.c_str(), killName.c_str());

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
    std::string filepathS = (setDir+"/")+setS+".bin";
    std::vector<char> objectsSetS = readAllBytes(filepathS.c_str());
    for (int i = 0; i < (int)objectsSetS.size(); i+=32)
    {
        processObjectSET(&objectsSetS[i]);
    }

    //now run through the binary set files:
    std::string filepathU = (setDir+"/")+setU+".bin";
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
                case 0:  Global::addEntity(new RING       (data)); INCR_NEW("Entity"); break;
                case 1:  Global::addEntity(new RING_LINEAR(data)); INCR_NEW("Entity"); break;
                default: Global::addEntity(new Unknown    (data)); INCR_NEW("Entity"); break;
            }
            break;

        case Global::Levels::Metal_Harbor:
            switch (objectType)
            {
                case 0:  Global::addEntity(new RING       (data)); INCR_NEW("Entity"); break;
                case 1:  Global::addEntity(new RING_LINEAR(data)); INCR_NEW("Entity"); break;
                default: Global::addEntity(new Unknown    (data)); INCR_NEW("Entity"); break;
            }
            break;

        case Global::Levels::Sky_Rail:
            switch (objectType)
            {
                case 0:  Global::addEntity(new RING       (data)); INCR_NEW("Entity"); break;
                case 1:  Global::addEntity(new RING_LINEAR(data)); INCR_NEW("Entity"); break;
                default: Global::addEntity(new Unknown    (data)); INCR_NEW("Entity"); break;
            }
            break;

        case Global::Levels::Final_Rush:
            switch (objectType)
            {
                case 0:  Global::addEntity(new RING       (data)); INCR_NEW("Entity"); break;
                case 1:  Global::addEntity(new RING_LINEAR(data)); INCR_NEW("Entity"); break;
                default: Global::addEntity(new Unknown    (data)); INCR_NEW("Entity"); break;
            }
            break;

        case Global::Levels::Crazy_Gadget:
            switch (objectType)
            {
                case 0:  Global::addEntity(new RING       (data)); INCR_NEW("Entity"); break;
                case 1:  Global::addEntity(new RING_LINEAR(data)); INCR_NEW("Entity"); break;
                default: Global::addEntity(new Unknown    (data)); INCR_NEW("Entity"); break;
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
    Global::levelFileMap["Sky Rail"]     = "SkyRail.lvl2";
    Global::levelFileMap["Final Rush"]   = "FinalRush.lvl2";
    Global::levelFileMap["Crazy Gadget"] = "CrazyGadget.lvl2";

    Global::levelIDMap.clear();
    Global::levelIDMap[Global::Levels::Basic_Test           ] = "";
    Global::levelIDMap[Global::Levels::Knuckles_Test        ] = "";
    Global::levelIDMap[Global::Levels::Sonic_Test           ] = "";
    Global::levelIDMap[Global::Levels::Green_Forest         ] = "GreenForest.lvl2";
    Global::levelIDMap[Global::Levels::White_Jungle         ] = "";
    Global::levelIDMap[Global::Levels::Pumpkin_Hill         ] = "";
    Global::levelIDMap[Global::Levels::Sky_Rail             ] = "SkyRail.lvl2";
    Global::levelIDMap[Global::Levels::Aquatic_Mine         ] = "";
    Global::levelIDMap[Global::Levels::Security_Hall        ] = "";
    Global::levelIDMap[Global::Levels::Prison_Lane          ] = "";
    Global::levelIDMap[Global::Levels::Metal_Harbor         ] = "MetalHarbor.lvl2";
    Global::levelIDMap[Global::Levels::Iron_Gate            ] = "";
    Global::levelIDMap[Global::Levels::Weapons_Bed          ] = "";
    Global::levelIDMap[Global::Levels::City_Escape          ] = "";
    Global::levelIDMap[Global::Levels::Radical_Highway      ] = "";
    Global::levelIDMap[Global::Levels::Weapons_Bed_2P       ] = "";
    Global::levelIDMap[Global::Levels::Wild_Canyon          ] = "";
    Global::levelIDMap[Global::Levels::Mission_Street       ] = "";
    Global::levelIDMap[Global::Levels::Dry_Lagoon           ] = "";
    Global::levelIDMap[Global::Levels::Sonic_Vs_Shadow_1    ] = "";
    Global::levelIDMap[Global::Levels::Tails_Vs_Eggman_1    ] = "";
    Global::levelIDMap[Global::Levels::Sand_Ocean           ] = "";
    Global::levelIDMap[Global::Levels::Crazy_Gadget         ] = "CrazyGadget.lvl2";
    Global::levelIDMap[Global::Levels::Hidden_Base          ] = "";
    Global::levelIDMap[Global::Levels::Eternal_Engine       ] = "";
    Global::levelIDMap[Global::Levels::Death_Chamber        ] = "";
    Global::levelIDMap[Global::Levels::Egg_Quarters         ] = "";
    Global::levelIDMap[Global::Levels::Lost_Colony          ] = "";
    Global::levelIDMap[Global::Levels::Pyramid_Cave         ] = "";
    Global::levelIDMap[Global::Levels::Tails_Vs_Eggman_2    ] = "";
    Global::levelIDMap[Global::Levels::Final_Rush           ] = "FinalRush.lvl2";
    Global::levelIDMap[Global::Levels::Green_Hill           ] = "";
    Global::levelIDMap[Global::Levels::Meteor_Herd          ] = "";
    Global::levelIDMap[Global::Levels::Knuckles_Vs_Rouge    ] = "";
    Global::levelIDMap[Global::Levels::Cannons_Core_Sonic   ] = "";
    Global::levelIDMap[Global::Levels::Cannons_Core_Eggman  ] = "";
    Global::levelIDMap[Global::Levels::Cannons_Core_Tails   ] = "";
    Global::levelIDMap[Global::Levels::Cannons_Core_Rouge   ] = "";
    Global::levelIDMap[Global::Levels::Cannons_Core_Knuckles] = "";
    Global::levelIDMap[Global::Levels::Mission_Street_2P    ] = "";
    Global::levelIDMap[Global::Levels::Final_Chase          ] = "";
    Global::levelIDMap[Global::Levels::Wild_Canyon_2P       ] = "";
    Global::levelIDMap[Global::Levels::Sonic_Vs_Shadow_2    ] = "";
    Global::levelIDMap[Global::Levels::Cosmic_Wall          ] = "";
    Global::levelIDMap[Global::Levels::Mad_Space            ] = "";
    Global::levelIDMap[Global::Levels::Sand_Ocean_2P        ] = "";
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

void LevelLoader::promptUserForLevel()
{
    #if defined(_WIN32)
    int response = MessageBox(NULL, 
                              "Load a new level? Unsaved progress will be lost!", 
                              "Load New Level", 
                              MB_YESNO);
    if (response != IDYES)
    {
        return;
    }

    const int BUFSIZE = 1024;
    char filePaths[BUFSIZE] = {0};
    OPENFILENAME ofns = {0};
    ofns.lStructSize = sizeof(OPENFILENAME);
    ofns.lpstrFile = filePaths;
    ofns.nMaxFile = BUFSIZE;
    ofns.lpstrInitialDir = (Global::dirSA2Root + "\\resource\\gd_PC\\").c_str();
    ofns.lpstrFilter = "SET files (set*_*.bin)\0set*_*.bin\0";
    ofns.nFilterIndex = 1; //default filter to show
    ofns.lpstrTitle = "Select BOTH setXXXX_s AND setXXXX_u for the level you want to edit";
    ofns.Flags = OFN_NOCHANGEDIR | OFN_ALLOWMULTISELECT | OFN_EXPLORER;

    bool opened = GetOpenFileName(&ofns);
    if (!opened)
    {
        return;
    }

    std::string folder = &filePaths[0];
    std::string file1 = &filePaths[ofns.nFileOffset];
    std::string file2 = "";

    for (int i = ofns.nFileOffset; i < BUFSIZE; i++)
    {
        if (filePaths[i] == 0)
        {
            if (filePaths[i+1] == 0)
            {
                MessageBox(NULL, "You only selected one file. Please select both setXXXX_s.bin and setXXXX_u.bin", "silly", NULL);
                return;
            }
            else
            {
                file2 = &filePaths[i+1];
                break;
            }
        }
    }

    int len1 = (int)file1.length();
    int len2 = (int)file2.length();

    if (len1 < 13 || len2 < 13)
    {
        MessageBox(NULL, "File length doesn't look like it is long enough. Please select both setXXXX_s.bin and setXXXX_u.bin", "silly", NULL);
        return;
    }

    if (file1.c_str()[0] != 's' ||
        file1.c_str()[1] != 'e' ||
        file1.c_str()[2] != 't' || 
        file2.c_str()[0] != 's' ||
        file2.c_str()[1] != 'e' ||
        file2.c_str()[2] != 't')
    {
        MessageBox(NULL, "File name doesn't start with 'set'. Please select both setXXXX_s.bin and setXXXX_u.bin", "silly", NULL);
        return;
    }

    char lvlDI1[5] = {0};
    char lvlDI2[5] = {0};
    memcpy(lvlDI1, &file1.c_str()[3], 4);
    memcpy(lvlDI2, &file2.c_str()[3], 4);

    int level1 = -1;
    int level2 = -2;

    try
    {
        level1 = std::stoi(lvlDI1);
        level2 = std::stoi(lvlDI2);
    }
    catch (...)
    {
        MessageBox(NULL, "It seems the files you selected could not be parsed for a level ID", "silly", NULL);
        return;
    }

    if (level1 != level2)
    {
        MessageBox(NULL, "It seems you selected different levels", "silly", NULL);
        return;
    }

    LevelLoader::loadLevel(level1, folder);
    Global::redrawWindow = true;
    #endif
}
