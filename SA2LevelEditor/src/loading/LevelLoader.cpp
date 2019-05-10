#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <list>

#include <chrono>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "levelloader.h"
#include "objLoader.h"
#include "loader.h"
#include "../main/main.h"
#include "../collision/collisionchecker.h"
#include "../collision/collisionmodel.h"
#include "../toolbox/getline.h"
#include "../toolbox/split.h"
#include "../toolbox/input.h"
#include "../toolbox/readbytes.h"
#include "../entities/stage.h"
#include "../entities/stagepass2.h"
#include "../entities/stagepass3.h"
#include "../entities/stagetransparent.h"
#include "../entities/stagecollision.h"
#include "../entities/stagekillplanes.h"
#include "../entities/stagesky.h"
#include "../entities/camera.h"
#include "../entities/sa2object.h"
#include "../entities/unknown.h"
#include "../entities/GlobalObjects/ring.h"
#include "../entities/GlobalObjects/ringlinear.h"
#include "../entities/GlobalObjects/ringcircle.h"
#include "../entities/GlobalObjects/sprb.h"
#include "../entities/GlobalObjects/spra.h"
#include "../entities/GlobalObjects/kasoku.h"
#include "../entities/GlobalObjects/ccube.h"
#include "../entities/GlobalObjects/ccyl.h"
#include "../entities/GlobalObjects/sphere.h"
#include "../entities/GlobalObjects/emerald.h"
#include "../entities/GlobalObjects/bigjump.h"

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

void LevelLoader::loadLevel(std::string setDir, std::string setS, std::string setU)
{
    std::string fname = "";

    if (Global::levelSetToLVL2.find(setS) == Global::levelSetToLVL2.end() ||
        Global::levelSetToLVL2.find(setU) == Global::levelSetToLVL2.end())
    {
        std::fprintf(stderr, "Trying to load a non existing level.\n");
        return;
    }

	fname = "res/Levels/"+Global::levelSetToLVL2[setS];
    Global::dirSet = setDir;

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
    StageSky::deleteStaticModels();

    CollisionChecker::deleteAllCollideModels();

    Global::gameCamera->reset();
    Global::selectedSA2Object = nullptr;
    Global::resetObjectWindow();


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
    std::string setSname;
	getlineSafe(file, setSname);
    std::string setUname;
	getlineSafe(file, setUname);
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


    //sky directory
    std::string skyDir;
	getlineSafe(file, skyDir);
    //sky model name
    std::string skyName;
	getlineSafe(file, skyName);
    StageSky::loadModels(skyDir.c_str(), skyName.c_str());

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
    std::string filepathS = setDir+setS;
    std::vector<char> objectsSetS = readAllBytes(filepathS.c_str());
    for (int i = 32; i < (int)objectsSetS.size(); i+=32)
    {
        processObjectSET(&objectsSetS[i]);
    }

    //now run through the binary set files:
    std::string filepathU = setDir+setU;
    std::vector<char> objectsSetU = readAllBytes(filepathU.c_str());
    for (int i = 32; i < (int)objectsSetU.size(); i+=32)
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
    #ifdef OBS_MODE
    return;
    #endif

    SA2Object* newObject = newSA2Object(Global::levelID, data[1], data, false);
    if (newObject != nullptr)
    {
        Global::addEntity(newObject);
    }
}

SA2Object* LevelLoader::newSA2Object(int levelID, int objectID, char data[32], bool useDefaultValues)
{
    if (objectID != (int)data[1] || objectID < 0)
    {
        return nullptr;
    }

    INCR_NEW("Entity");

    switch (levelID)
    {
        case Global::Levels::Metal_Harbor:
            switch (objectID)
            {
                case  0: return new RING       (data, useDefaultValues);
                case  1: return new RING_LINEAR(data, useDefaultValues);
                case  2: return new RING_CIRCLE(data, useDefaultValues);
                case  3: return new SPRA       (data, useDefaultValues);
                case  6: return new BIGJUMP    (data, useDefaultValues);
                case  7: return new KASOKU     (data, useDefaultValues);
                case  4: return new SPRB       (data, useDefaultValues);
                case 46: return new SPHERE     (data, useDefaultValues);
                case 47: return new CCYL       (data, useDefaultValues);
                case 48: return new CCUBE      (data, useDefaultValues);
                default: return new Unknown    (data, useDefaultValues);
            }

        case Global::Levels::Green_Forest:
            switch (objectID)
            {
                case  0: return new RING       (data, useDefaultValues);
                case  1: return new SPRA       (data, useDefaultValues);
                case  2: return new SPRB       (data, useDefaultValues);
                case  3: return new KASOKU     (data, useDefaultValues);
                case 10: return new BIGJUMP    (data, useDefaultValues);
                case 22: return new RING_LINEAR(data, useDefaultValues);
                case 56: return new RING_CIRCLE(data, useDefaultValues);
                case 66: return new SPHERE     (data, useDefaultValues);
                case 67: return new CCYL       (data, useDefaultValues);
                case 68: return new CCUBE      (data, useDefaultValues);
                default: return new Unknown    (data, useDefaultValues);
            }

        case Global::Levels::Crazy_Gadget:
            switch (objectID)
            {
                case  0: return new RING       (data, useDefaultValues);
                case  1: return new RING_LINEAR(data, useDefaultValues);
                case  2: return new RING_CIRCLE(data, useDefaultValues);
                case  3: return new SPRA       (data, useDefaultValues);
                case  4: return new SPRB       (data, useDefaultValues);
                case  6: return new BIGJUMP    (data, useDefaultValues);
                case  7: return new KASOKU     (data, useDefaultValues);
                case 44: return new SPHERE     (data, useDefaultValues);
                case 45: return new CCYL       (data, useDefaultValues);
                case 46: return new CCUBE      (data, useDefaultValues);
                default: return new Unknown    (data, useDefaultValues);
            }

        case Global::Levels::Final_Rush:
            switch (objectID)
            {
                case  0: return new RING       (data, useDefaultValues);
                case  1: return new RING_LINEAR(data, useDefaultValues);
                case  2: return new RING_CIRCLE(data, useDefaultValues);
                case  3: return new SPRA       (data, useDefaultValues);
                case  4: return new SPRB       (data, useDefaultValues);
                case  6: return new BIGJUMP    (data, useDefaultValues);
                case  7: return new KASOKU     (data, useDefaultValues);
                case 46: return new SPHERE     (data, useDefaultValues);
                case 47: return new CCYL       (data, useDefaultValues);
                case 48: return new CCUBE      (data, useDefaultValues);
                default: return new Unknown    (data, useDefaultValues);
            }

        case Global::Levels::Meteor_Herd:
            switch (objectID)
            {
                case    0: return new RING       (data, useDefaultValues);
                case    1: return new RING_LINEAR(data, useDefaultValues);
                case    2: return new RING_CIRCLE(data, useDefaultValues);
                case    3: return new SPRA       (data, useDefaultValues);
                case    4: return new SPRB       (data, useDefaultValues);
                case    6: return new BIGJUMP    (data, useDefaultValues);
                case    7: return new KASOKU     (data, useDefaultValues);
                case 0x0F: return new EMERALD    (data, useDefaultValues);
                case 0x2E: return new SPHERE     (data, useDefaultValues);
                case 0x2F: return new CCYL       (data, useDefaultValues);
                case 0x30: return new CCUBE      (data, useDefaultValues);
                default:   return new Unknown    (data, useDefaultValues);
            }

        case Global::Levels::Sky_Rail:
            switch (objectID)
            {
                case    0: return new RING       (data, useDefaultValues);
                case    1: return new SPRA       (data, useDefaultValues);
                case    2: return new SPRB       (data, useDefaultValues);
                case    3: return new KASOKU     (data, useDefaultValues);
                case    9: return new BIGJUMP    (data, useDefaultValues);
                case 0x11: return new RING_LINEAR(data, useDefaultValues);
                case 0x12: return new RING_CIRCLE(data, useDefaultValues);
                case 0x4A: return new SPHERE     (data, useDefaultValues);
                case 0x4B: return new CCYL       (data, useDefaultValues);
                case 0x4C: return new CCUBE      (data, useDefaultValues);
                default:   return new Unknown    (data, useDefaultValues);
            }

        case Global::Levels::Final_Chase:
            switch (objectID)
            {
                case  0: return new RING       (data, useDefaultValues);
                case  1: return new RING_LINEAR(data, useDefaultValues);
                case  2: return new RING_CIRCLE(data, useDefaultValues);
                case  3: return new SPRA       (data, useDefaultValues);
                case  4: return new SPRB       (data, useDefaultValues);
                case  6: return new BIGJUMP    (data, useDefaultValues);
                case  7: return new KASOKU     (data, useDefaultValues);
                case 46: return new SPHERE     (data, useDefaultValues);
                case 47: return new CCYL       (data, useDefaultValues);
                case 48: return new CCUBE      (data, useDefaultValues);
                default: return new Unknown    (data, useDefaultValues);
            }

        case Global::Levels::Cosmic_Wall:
            switch (objectID)
            {
                case  0: return new RING       (data, useDefaultValues);
                case  1: return new RING_LINEAR(data, useDefaultValues);
                case  2: return new RING_CIRCLE(data, useDefaultValues);
                case  3: return new SPRA       (data, useDefaultValues);
                case  4: return new SPRB       (data, useDefaultValues);
                case  6: return new BIGJUMP    (data, useDefaultValues);
                case  7: return new KASOKU     (data, useDefaultValues);
                case 38: return new SPHERE     (data, useDefaultValues);
                case 39: return new CCYL       (data, useDefaultValues);
                case 40: return new CCUBE      (data, useDefaultValues);
                default: return new Unknown    (data, useDefaultValues);
            }

        case Global::Levels::Dry_Lagoon:
            switch (objectID)
            {
                case    0: return new RING       (data, useDefaultValues);
                case    1: return new RING_LINEAR(data, useDefaultValues);
                case    2: return new RING_CIRCLE(data, useDefaultValues);
                case    3: return new SPRA       (data, useDefaultValues);
                case    4: return new SPRB       (data, useDefaultValues);
                case    6: return new BIGJUMP    (data, useDefaultValues);
                case    7: return new KASOKU     (data, useDefaultValues);
                case 0x0F: return new EMERALD    (data, useDefaultValues);
                case 0x2E: return new SPHERE     (data, useDefaultValues);
                case 0x2F: return new CCYL       (data, useDefaultValues);
                case 0x30: return new CCUBE      (data, useDefaultValues);
                default:   return new Unknown    (data, useDefaultValues);
            }

        case Global::Levels::Security_Hall:
            switch (objectID)
            {
                case    0: return new RING       (data, useDefaultValues);
                case    1: return new RING_LINEAR(data, useDefaultValues);
                case    2: return new RING_CIRCLE(data, useDefaultValues);
                case    3: return new SPRA       (data, useDefaultValues);
                case    4: return new SPRB       (data, useDefaultValues);
                case    6: return new BIGJUMP    (data, useDefaultValues);
                case    7: return new KASOKU     (data, useDefaultValues);
                case 0x0F: return new EMERALD    (data, useDefaultValues);
                case 0x2E: return new SPHERE     (data, useDefaultValues);
                case 0x2F: return new CCYL       (data, useDefaultValues);
                case 0x30: return new CCUBE      (data, useDefaultValues);
                default:   return new Unknown    (data, useDefaultValues);
            }

        case Global::Levels::Mad_Space:
            switch (objectID)
            {
                case    0: return new RING       (data, useDefaultValues);
                case    1: return new RING_LINEAR(data, useDefaultValues);
                case    2: return new RING_CIRCLE(data, useDefaultValues);
                case    3: return new SPRA       (data, useDefaultValues);
                case    4: return new SPRB       (data, useDefaultValues);
                case    6: return new BIGJUMP    (data, useDefaultValues);
                case    7: return new KASOKU     (data, useDefaultValues);
                case 0x0F: return new EMERALD    (data, useDefaultValues);
                case 0x2D: return new SPHERE     (data, useDefaultValues);
                case 0x2E: return new CCYL       (data, useDefaultValues);
                case 0x2F: return new CCUBE      (data, useDefaultValues);
                default:   return new Unknown    (data, useDefaultValues);
            }

        default:
            INCR_DEL("Entity");
            std::fprintf(stdout, "Error: level not supported yet.\n");
            break;
    }

    return nullptr;
}

void LevelLoader::loadLevelData()
{
    Global::levelSetToLVL2.clear();

    Global::levelSetToLVL2["set_b_bigbogy_s.bin"]  = "";
    Global::levelSetToLVL2["set_b_bigbogy_u.bin"]  = "";
    Global::levelSetToLVL2["set_b_bigfoot_s.bin"]  = "";
    Global::levelSetToLVL2["set_b_bigfoot_u.bin"]  = "";
    Global::levelSetToLVL2["set_b_fdog_s.bin"]     = "";
    Global::levelSetToLVL2["set_b_fdog_u.bin"]     = "";
    Global::levelSetToLVL2["set_b_Golem_e_s.bin"]  = "";
    Global::levelSetToLVL2["set_b_Golem_e_u.bin"]  = "";
    Global::levelSetToLVL2["set_b_Golem_s.bin"]    = "";
    Global::levelSetToLVL2["set_b_Golem_u.bin"]    = "";
    Global::levelSetToLVL2["set_b_hotshot_s.bin"]  = "";
    Global::levelSetToLVL2["set_b_hotshot_u.bin"]  = "";
    Global::levelSetToLVL2["set_b_last1_s.bin"]    = "";
    Global::levelSetToLVL2["set_b_last1_u.bin"]    = "";
    Global::levelSetToLVL2["set_chao_dark.bin"]    = "";
    Global::levelSetToLVL2["set_chao_dark_s.bin"]  = "";
    Global::levelSetToLVL2["set_chao_dark_u.bin"]  = "";
    Global::levelSetToLVL2["set_chao_hero.bin"]    = "";
    Global::levelSetToLVL2["set_chao_hero_s.bin"]  = "";
    Global::levelSetToLVL2["set_chao_hero_u.bin"]  = "";
    Global::levelSetToLVL2["set_chao_neut.bin"]    = "";
    Global::levelSetToLVL2["set_chao_neut_s.bin"]  = "";
    Global::levelSetToLVL2["set_chao_neut_u.bin"]  = "";
    Global::levelSetToLVL2["SET0000_S.bin"]        = "";
    Global::levelSetToLVL2["SET0000_U.bin"]        = "";
    Global::levelSetToLVL2["set0001_s.bin"]        = "";
    Global::levelSetToLVL2["set0001_u.bin"]        = "";
    Global::levelSetToLVL2["set0003_2p_s.bin"]     = "GreenForest.lvl2";
    Global::levelSetToLVL2["set0003_2p_u.bin"]     = "GreenForest.lvl2";
    Global::levelSetToLVL2["set0003_hd_s.bin"]     = "GreenForest.lvl2";
    Global::levelSetToLVL2["set0003_hd_u.bin"]     = "GreenForest.lvl2";
    Global::levelSetToLVL2["set0003_s.bin"]        = "GreenForest.lvl2";
    Global::levelSetToLVL2["set0003_u.bin"]        = "GreenForest.lvl2";
    Global::levelSetToLVL2["set0004_2p_s.bin"]     = "";
    Global::levelSetToLVL2["set0004_2p_u.bin"]     = "";
    Global::levelSetToLVL2["set0004_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0004_s.bin"]        = "";
    Global::levelSetToLVL2["set0004_u.bin"]        = "";
    Global::levelSetToLVL2["set0005_2P_s.bin"]     = "";
    Global::levelSetToLVL2["set0005_2P_u.bin"]     = "";
    Global::levelSetToLVL2["set0005_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0005_s.bin"]        = "";
    Global::levelSetToLVL2["set0005_u.bin"]        = "";
    Global::levelSetToLVL2["set0006_2p_s.bin"]     = "SkyRail.lvl2";
    Global::levelSetToLVL2["set0006_2p_u.bin"]     = "SkyRail.lvl2";
    Global::levelSetToLVL2["set0006_hd_s.bin"]     = "SkyRail.lvl2";
    Global::levelSetToLVL2["set0006_hd_u.bin"]     = "SkyRail.lvl2";
    Global::levelSetToLVL2["set0006_s.bin"]        = "SkyRail.lvl2";
    Global::levelSetToLVL2["set0006_u.bin"]        = "SkyRail.lvl2";
    Global::levelSetToLVL2["set0007_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0007_hd_u.bin"]     = "";
    Global::levelSetToLVL2["set0007_s.bin"]        = "";
    Global::levelSetToLVL2["set0007_u.bin"]        = "";
    Global::levelSetToLVL2["set0008_2p_s.bin"]     = "SecurityHall.lvl2";
    Global::levelSetToLVL2["set0008_2p_u.bin"]     = "SecurityHall.lvl2";
    Global::levelSetToLVL2["set0008_hd_s.bin"]     = "SecurityHall.lvl2";
    Global::levelSetToLVL2["set0008_s.bin"]        = "SecurityHall.lvl2";
    Global::levelSetToLVL2["set0008_u.bin"]        = "SecurityHall.lvl2";
    Global::levelSetToLVL2["set0009_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0009_s.bin"]        = "";
    Global::levelSetToLVL2["set0009_u.bin"]        = "";
    Global::levelSetToLVL2["set0010_2p_u.bin"]     = "MetalHarbor.lvl2";
    Global::levelSetToLVL2["set0010_hd_s.bin"]     = "MetalHarbor.lvl2";
    Global::levelSetToLVL2["set0010_hd_u.bin"]     = "MetalHarbor.lvl2";
    Global::levelSetToLVL2["set0010_s.bin"]        = "MetalHarbor.lvl2";
    Global::levelSetToLVL2["set0010_u.bin"]        = "MetalHarbor.lvl2";
    Global::levelSetToLVL2["set0011_2p_s.bin"]     = "";
    Global::levelSetToLVL2["set0011_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0011_hd_u.bin"]     = "";
    Global::levelSetToLVL2["set0011_s.bin"]        = "";
    Global::levelSetToLVL2["set0011_u.bin"]        = "";
    Global::levelSetToLVL2["set0012_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0012_hd_u.bin"]     = "";
    Global::levelSetToLVL2["set0012_s.bin"]        = "";
    Global::levelSetToLVL2["set0012_u.bin"]        = "";
    Global::levelSetToLVL2["set0013_2p_s.bin"]     = "";
    Global::levelSetToLVL2["set0013_2p_u.bin"]     = "";
    Global::levelSetToLVL2["set0013_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0013_s.bin"]        = "";
    Global::levelSetToLVL2["set0013_u.bin"]        = "";
    Global::levelSetToLVL2["set0014_2p_s.bin"]     = "";
    Global::levelSetToLVL2["set0014_2p_u.bin"]     = "";
    Global::levelSetToLVL2["set0014_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0014_s.bin"]        = "";
    Global::levelSetToLVL2["set0014_u.bin"]        = "";
    Global::levelSetToLVL2["set0015_s.bin"]        = "";
    Global::levelSetToLVL2["set0015_u.bin"]        = "";
    Global::levelSetToLVL2["set0016_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0016_hd_u.bin"]     = "";
    Global::levelSetToLVL2["set0016_s.bin"]        = "";
    Global::levelSetToLVL2["set0016_u.bin"]        = "";
    Global::levelSetToLVL2["set0017_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0017_s.bin"]        = "";
    Global::levelSetToLVL2["set0017_u.bin"]        = "";
    Global::levelSetToLVL2["set0018_hd_s.bin"]     = "DryLagoon.lvl2";
    Global::levelSetToLVL2["set0018_hd_u.bin"]     = "DryLagoon.lvl2";
    Global::levelSetToLVL2["set0018_s.bin"]        = "DryLagoon.lvl2";
    Global::levelSetToLVL2["set0018_u.bin"]        = "DryLagoon.lvl2";
    Global::levelSetToLVL2["set0019_s.bin"]        = "";
    Global::levelSetToLVL2["SET0019_U.bin"]        = "";
    Global::levelSetToLVL2["SET0020_S.bin"]        = "";
    Global::levelSetToLVL2["SET0020_U.bin"]        = "";
    Global::levelSetToLVL2["set0021_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0021_hd_u.bin"]     = "";
    Global::levelSetToLVL2["set0021_s.bin"]        = "";
    Global::levelSetToLVL2["set0021_u.bin"]        = "";
    Global::levelSetToLVL2["set0022_hd_s.bin"]     = "CrazyGadget.lvl2";
    Global::levelSetToLVL2["set0022_s.bin"]        = "CrazyGadget.lvl2";
    Global::levelSetToLVL2["set0022_u.bin"]        = "CrazyGadget.lvl2";
    Global::levelSetToLVL2["set0023_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0023_hd_u.bin"]     = "";
    Global::levelSetToLVL2["set0023_s.bin"]        = "";
    Global::levelSetToLVL2["set0023_u.bin"]        = "";
    Global::levelSetToLVL2["set0024_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0024_s.bin"]        = "";
    Global::levelSetToLVL2["SET0024_U.bin"]        = "";
    Global::levelSetToLVL2["set0025_2p_s.bin"]     = "";
    Global::levelSetToLVL2["set0025_2p_u.bin"]     = "";
    Global::levelSetToLVL2["set0025_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0025_s.bin"]        = "";
    Global::levelSetToLVL2["set0025_u.bin"]        = "";
    Global::levelSetToLVL2["set0026_2p_s.bin"]     = "";
    Global::levelSetToLVL2["set0026_2p_u.bin"]     = "";
    Global::levelSetToLVL2["set0026_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0026_s.bin"]        = "";
    Global::levelSetToLVL2["set0026_u.bin"]        = "";
    Global::levelSetToLVL2["set0027_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0027_s.bin"]        = "";
    Global::levelSetToLVL2["set0027_u.bin"]        = "";
    Global::levelSetToLVL2["set0028_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0028_hd_u.bin"]     = "";
    Global::levelSetToLVL2["set0028_s.bin"]        = "";
    Global::levelSetToLVL2["set0028_u.bin"]        = "";
    Global::levelSetToLVL2["set0029_s.bin"]        = "";
    Global::levelSetToLVL2["set0029_u.bin"]        = "";
    Global::levelSetToLVL2["set0030_2p_s.bin"]     = "FinalRush.lvl2";
    Global::levelSetToLVL2["set0030_2p_u.bin"]     = "FinalRush.lvl2";
    Global::levelSetToLVL2["set0030_hd_s.bin"]     = "FinalRush.lvl2";
    Global::levelSetToLVL2["set0030_hd_u.bin"]     = "FinalRush.lvl2";
    Global::levelSetToLVL2["set0030_s.bin"]        = "FinalRush.lvl2";
    Global::levelSetToLVL2["set0030_u.bin"]        = "FinalRush.lvl2";
    Global::levelSetToLVL2["set0031_s.bin"]        = "";
    Global::levelSetToLVL2["set0031_u.bin"]        = "";
    Global::levelSetToLVL2["set0032_2p_s.bin"]     = "MeteorHerd.lvl2";
    Global::levelSetToLVL2["set0032_2p_u.bin"]     = "MeteorHerd.lvl2";
    Global::levelSetToLVL2["set0032_hd_s.bin"]     = "MeteorHerd.lvl2";
    Global::levelSetToLVL2["set0032_hd_u.bin"]     = "MeteorHerd.lvl2";
    Global::levelSetToLVL2["set0032_s.bin"]        = "MeteorHerd.lvl2";
    Global::levelSetToLVL2["set0032_u.bin"]        = "MeteorHerd.lvl2";
    Global::levelSetToLVL2["set0033_s.bin"]        = "";
    Global::levelSetToLVL2["set0033_u.bin"]        = "";
    Global::levelSetToLVL2["set0034_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0034_s.bin"]        = "";
    Global::levelSetToLVL2["set0034_u.bin"]        = "";
    Global::levelSetToLVL2["set0035_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0035_s.bin"]        = "";
    Global::levelSetToLVL2["set0035_u.bin"]        = "";
    Global::levelSetToLVL2["set0036_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0036_s.bin"]        = "";
    Global::levelSetToLVL2["set0036_u.bin"]        = "";
    Global::levelSetToLVL2["set0037_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0037_s.bin"]        = "";
    Global::levelSetToLVL2["set0037_u.bin"]        = "";
    Global::levelSetToLVL2["set0038_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0038_s.bin"]        = "";
    Global::levelSetToLVL2["set0038_u.bin"]        = "";
    Global::levelSetToLVL2["set0039_s.bin"]        = "";
    Global::levelSetToLVL2["SET0039_U.bin"]        = "";
    Global::levelSetToLVL2["set0040_2p_s.bin"]     = "FinalChase.lvl2";
    Global::levelSetToLVL2["set0040_2p_u.bin"]     = "FinalChase.lvl2";
    Global::levelSetToLVL2["set0040_hd_s.bin"]     = "FinalChase.lvl2";
    Global::levelSetToLVL2["set0040_hd_u.bin"]     = "FinalChase.lvl2";
    Global::levelSetToLVL2["set0040_s.bin"]        = "FinalChase.lvl2";
    Global::levelSetToLVL2["set0040_u.bin"]        = "FinalChase.lvl2";
    Global::levelSetToLVL2["SET0041_2P_S.bin"]     = "";
    Global::levelSetToLVL2["set0041_2p_u.bin"]     = "";
    Global::levelSetToLVL2["SET0042_S.bin"]        = "";
    Global::levelSetToLVL2["set0042_u.bin"]        = "";
    Global::levelSetToLVL2["set0043_hd_s.bin"]     = "CosmicWall.lvl2";
    Global::levelSetToLVL2["set0043_hd_u.bin"]     = "CosmicWall.lvl2";
    Global::levelSetToLVL2["set0043_s.bin"]        = "CosmicWall.lvl2";
    Global::levelSetToLVL2["set0043_u.bin"]        = "CosmicWall.lvl2";
    Global::levelSetToLVL2["set0044_2p_s.bin"]     = "MadSpace.lvl2";
    Global::levelSetToLVL2["set0044_2p_u.bin"]     = "MadSpace.lvl2";
    Global::levelSetToLVL2["set0044_hd_s.bin"]     = "MadSpace.lvl2";
    Global::levelSetToLVL2["set0044_hd_u.bin"]     = "MadSpace.lvl2";
    Global::levelSetToLVL2["set0044_s.bin"]        = "MadSpace.lvl2";
    Global::levelSetToLVL2["set0044_u.bin"]        = "MadSpace.lvl2";
    Global::levelSetToLVL2["set0045_s.bin"]        = "";
    Global::levelSetToLVL2["set0045_u.bin"]        = "";
    Global::levelSetToLVL2["SET0046_2P_S.bin"]     = "";
    Global::levelSetToLVL2["SET0046_2P_U.bin"]     = "";
    Global::levelSetToLVL2["SET0046_S.bin"]        = "";
    Global::levelSetToLVL2["SET0046_U.bin"]        = "";
    Global::levelSetToLVL2["SET0047_2P_S.bin"]     = "";
    Global::levelSetToLVL2["SET0047_2P_U.bin"]     = "";
    Global::levelSetToLVL2["SET0047_S.bin"]        = "";
    Global::levelSetToLVL2["SET0047_U.bin"]        = "";
    Global::levelSetToLVL2["SET0048_2P_S.bin"]     = "";
    Global::levelSetToLVL2["SET0048_2P_U.bin"]     = "";
    Global::levelSetToLVL2["SET0048_S.bin"]        = "";
    Global::levelSetToLVL2["SET0048_U.bin"]        = "";
    Global::levelSetToLVL2["SET0049_2P_S.bin"]     = "";
    Global::levelSetToLVL2["SET0049_2P_U.bin"]     = "";
    Global::levelSetToLVL2["SET0049_S.bin"]        = "";
    Global::levelSetToLVL2["SET0049_U.bin"]        = "";
    Global::levelSetToLVL2["SET0050_2P_S.bin"]     = "";
    Global::levelSetToLVL2["SET0050_2P_U.bin"]     = "";
    Global::levelSetToLVL2["SET0050_S.bin"]        = "";
    Global::levelSetToLVL2["SET0050_U.bin"]        = "";
    Global::levelSetToLVL2["SET0051_2P_S.bin"]     = "";
    Global::levelSetToLVL2["SET0051_2P_U.bin"]     = "";
    Global::levelSetToLVL2["SET0051_S.bin"]        = "";
    Global::levelSetToLVL2["SET0051_U.bin"]        = "";
    Global::levelSetToLVL2["set0052_s.bin"]        = "";
    Global::levelSetToLVL2["set0052_u.bin"]        = "";
    Global::levelSetToLVL2["set0053_s.bin"]        = "";
    Global::levelSetToLVL2["set0053_u.bin"]        = "";
    Global::levelSetToLVL2["set0054_s.bin"]        = "";
    Global::levelSetToLVL2["set0054_u.bin"]        = "";
    Global::levelSetToLVL2["set0055_s.bin"]        = "";
    Global::levelSetToLVL2["set0055_u.bin"]        = "";
    Global::levelSetToLVL2["set0056_s.bin"]        = "";
    Global::levelSetToLVL2["set0056_u.bin"]        = "";
    Global::levelSetToLVL2["set0057_2p_s.bin"]     = "";
    Global::levelSetToLVL2["set0057_2p_u.bin"]     = "";
    Global::levelSetToLVL2["set0058_2p_s.bin"]     = "";
    Global::levelSetToLVL2["set0058_2p_u.bin"]     = "";
    Global::levelSetToLVL2["set0058_s.bin"]        = "";
    Global::levelSetToLVL2["set0058_u.bin"]        = "";
    Global::levelSetToLVL2["set0059_s.bin"]        = "";
    Global::levelSetToLVL2["set0059_u.bin"]        = "";
    Global::levelSetToLVL2["setCartBase.bin"]      = "";
    Global::levelSetToLVL2["setCartMini1.bin"]     = "";
    Global::levelSetToLVL2["setCartMini2.bin"]     = "";
    Global::levelSetToLVL2["setCartMini3.bin"]     = "";
    Global::levelSetToLVL2["setCartRouge.bin"]     = "";
    Global::levelSetToLVL2["setCartRouge_hd.bin"]  = "";
    Global::levelSetToLVL2["setCartTails.bin"]     = "";
    Global::levelSetToLVL2["setCartTails_hd.bin"]  = "";
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

    if (Global::levelSetToLVL2.find(file1) == Global::levelSetToLVL2.end())
    {
        MessageBox(NULL, (("This program does not recognize "+file1)+" to have a SA2 level associated with it.").c_str(), "silly", NULL);
        return;
    }
    if (Global::levelSetToLVL2.find(file2) == Global::levelSetToLVL2.end())
    {
        MessageBox(NULL, (("This program does not recognize "+file2)+" to have a SA2 level associated with it.").c_str(), "silly", NULL);
        return;
    }

    std::string lvl1 = Global::levelSetToLVL2[file1];
    std::string lvl2 = Global::levelSetToLVL2[file2];

    if (lvl1 != lvl2)
    {
        MessageBox(NULL, "These setfiles aren't associated with the same SA2 level.", "silly", NULL);
        return;
    }

    if (lvl1 == "" ||
        lvl2 == "")
    {
        MessageBox(NULL, "This level is not supported at this time.", "silly", NULL);
        return;
    }

    //int len1 = (int)file1.length();
    //int len2 = (int)file2.length();
    //
    //if (len1 < 13 || len2 < 13)
    //{
    //    MessageBox(NULL, "File length doesn't look like it is long enough. Please select both setXXXX_s.bin and setXXXX_u.bin", "silly", NULL);
    //    return;
    //}
    //
    //if (file1.c_str()[0] != 's' ||
    //    file1.c_str()[1] != 'e' ||
    //    file1.c_str()[2] != 't' || 
    //    file2.c_str()[0] != 's' ||
    //    file2.c_str()[1] != 'e' ||
    //    file2.c_str()[2] != 't')
    //{
    //    MessageBox(NULL, "File name doesn't start with 'set'. Please select both setXXXX_s.bin and setXXXX_u.bin", "silly", NULL);
    //    return;
    //}
    //
    //char lvlDI1[5] = {0};
    //char lvlDI2[5] = {0};
    //memcpy(lvlDI1, &file1.c_str()[3], 4);
    //memcpy(lvlDI2, &file2.c_str()[3], 4);
    //
    //int level1 = -1;
    //int level2 = -2;
    //
    //try
    //{
    //    level1 = std::stoi(lvlDI1);
    //    level2 = std::stoi(lvlDI2);
    //}
    //catch (...)
    //{
    //    MessageBox(NULL, "It seems the files you selected could not be parsed for a level ID", "silly", NULL);
    //    return;
    //}
    //
    //if (level1 != level2)
    //{
    //    MessageBox(NULL, "It seems you selected different levels", "silly", NULL);
    //    return;
    //}

    //std::fprintf(stdout, "dir = '%s'   s = '%s'   u = '%s'\n", (folder+"\\").c_str(), file1.c_str(), file2.c_str());
    LevelLoader::loadLevel((folder+"\\"), file1, file2);
    Global::redrawWindow = true;
    #endif
}

void LevelLoader::exportLevel()
{
    const int BUFSIZE = 1024;
    char filePaths[BUFSIZE] = {0};
    OPENFILENAME ofns = {0};
    ofns.lStructSize = sizeof(OPENFILENAME);
    ofns.lpstrFile = filePaths;
    ofns.nMaxFile = BUFSIZE;
    ofns.lpstrInitialDir = (Global::dirSA2Root + "\\resource\\gd_PC\\").c_str();
    ofns.lpstrFilter = "SET files (set*_*.bin)\0set*_*.bin\0";
    ofns.nFilterIndex = 1; //default filter to show
    ofns.lpstrTitle = "Select BOTH setXXXX_s AND setXXXX_u for the level you want to export to.";
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

    //find which one is u
    char lastChr1 = file1[file1.size() - 5];
    char lastChr2 = file2[file2.size() - 5];

    std::string fileS = "";
    std::string fileU = "";

    if (lastChr1 == 'u' && lastChr2 == 's')
    {
        fileS = file2;
        fileU = file1;
    }
    else if (lastChr1 == 's' && lastChr2 == 'u')
    {
        fileS = file1;
        fileU = file2;
    }
    else
    {
        MessageBox(NULL, "Cannot determine which file is the U file and which is the S file.", "silly", NULL);
        return;
    }

    //figure out all of the sa2 objects we need to write to the files
    std::vector<SA2Object*> sa2Objects;
    for (Entity* e : Global::gameEntities)
    {
        if (e->isSA2Object())
        {
            if (SA2Object* o = dynamic_cast<SA2Object*>(e))
            {
                sa2Objects.push_back(o);
            }
            else
            {
                std::fprintf(stdout, "Warning: object lied about being an sa2object\n");
            }
        }
    }

    if (sa2Objects.size() < 2)
    {
        std::fprintf(stdout, "Error: Less than 2 objects to export. Each setfile needs to have at least 1\n");
        MessageBox(NULL, "Error: Less than 2 objects to export. Each setfile needs to have at least 1", "silly", NULL);
        return;
    }

    //buffer to hold data for each sa2 object
    char buf[32] = {0};

    //the number of objects we will write to each setfile
    int countU = ((int)sa2Objects.size())/2;
    int countS = ((int)sa2Objects.size()) - countU;

    //write the U file
    std::ofstream myfileU((folder+"\\")+fileU, std::ios::out | std::ios::binary);
    if (!myfileU.is_open())
    {
        std::string exp = (folder+"\\")+fileU;
        MessageBox(NULL, (("Cannot open file '"+exp)+"' for writing.").c_str(), "silly", NULL);
        return;
    }

    //write the header to the U file
    memset(buf, 0, 32);
    buf[3] = (countU >>  0) & 0xFF;
    buf[2] = (countU >>  8) & 0xFF;
    buf[1] = (countU >> 16) & 0xFF;
    buf[0] = (countU >> 24) & 0xFF;
    myfileU.write(buf, 32);

    //write objects to the U file
    for (int i = 0; i < countU; i++)
    {
        SA2Object* o = sa2Objects[i];
        memset(buf, 0, 32);
        o->fillData(buf);
        buf[0] = (unsigned char)0x80; //first byte of U file is 0x80
        myfileU.write(buf, 32);
    }


    //write the S file as blank
    std::ofstream myfileS((folder+"\\")+fileS, std::ios::out | std::ios::binary);
    if (!myfileS.is_open())
    {
        std::string exp = (folder+"\\")+fileS;
        MessageBox(NULL, (("Cannot open file '"+exp)+"' for writing.").c_str(), "silly", NULL);
        return;
    }

    //write the header to the S file
    memset(buf, 0, 32);
    buf[3] = (countS >>  0) & 0xFF;
    buf[2] = (countS >>  8) & 0xFF;
    buf[1] = (countS >> 16) & 0xFF;
    buf[0] = (countS >> 24) & 0xFF;
    myfileS.write(buf, 32);

    //write objects to the S file
    for (int i = countU; i < sa2Objects.size(); i++)
    {
        SA2Object* o = sa2Objects[i];
        memset(buf, 0, 32);
        o->fillData(buf);
        buf[0] = 0x00; //first byte of S file is 0x00
        myfileS.write(buf, 32);
    }

    myfileS.close();
}
