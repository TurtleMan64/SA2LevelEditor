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
#include "../entities/GlobalObjects/ringcircle.h"
#include "../entities/GlobalObjects/sprb.h"
#include "../entities/GlobalObjects/spra.h"
#include "../toolbox/readbytes.h"
#include "loader.h"
#include "../entities/stagecollision.h"
#include "../entities/stagekillplanes.h"
#include "../entities/stagesky.h"

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
                case 1:  Global::addEntity(new SPRA       (data)); INCR_NEW("Entity"); break;
                case 2:  Global::addEntity(new SPRB       (data)); INCR_NEW("Entity"); break;
                case 22: Global::addEntity(new RING_LINEAR(data)); INCR_NEW("Entity"); break;
                case 56: Global::addEntity(new RING_CIRCLE(data)); INCR_NEW("Entity"); break;
                default: Global::addEntity(new Unknown    (data)); INCR_NEW("Entity"); break;
            }
            break;

        case Global::Levels::Metal_Harbor:
            switch (objectType)
            {
                case 0:  Global::addEntity(new RING       (data)); INCR_NEW("Entity"); break;
                case 1:  Global::addEntity(new RING_LINEAR(data)); INCR_NEW("Entity"); break;
                case 2:  Global::addEntity(new RING_CIRCLE(data)); INCR_NEW("Entity"); break;
                case 3:  Global::addEntity(new SPRA       (data)); INCR_NEW("Entity"); break;
                case 4:  Global::addEntity(new SPRB       (data)); INCR_NEW("Entity"); break;
                default: Global::addEntity(new Unknown    (data)); INCR_NEW("Entity"); break;
            }
            break;

        case Global::Levels::Sky_Rail:
            switch (objectType)
            {
                case 0:  Global::addEntity(new RING       (data)); INCR_NEW("Entity"); break;
                case 1:  Global::addEntity(new SPRA       (data)); INCR_NEW("Entity"); break;
                case 2:  Global::addEntity(new SPRB       (data)); INCR_NEW("Entity"); break;
                case 17: Global::addEntity(new RING_LINEAR(data)); INCR_NEW("Entity"); break;
                case 18: Global::addEntity(new RING_CIRCLE(data)); INCR_NEW("Entity"); break;
                default: Global::addEntity(new Unknown    (data)); INCR_NEW("Entity"); break;
            }
            break;

        case Global::Levels::Final_Rush:
            switch (objectType)
            {
                case 0:  Global::addEntity(new RING       (data)); INCR_NEW("Entity"); break;
                case 1:  Global::addEntity(new RING_LINEAR(data)); INCR_NEW("Entity"); break;
                case 2:  Global::addEntity(new RING_CIRCLE(data)); INCR_NEW("Entity"); break;
                case 3:  Global::addEntity(new SPRA       (data)); INCR_NEW("Entity"); break;
                case 4:  Global::addEntity(new SPRB       (data)); INCR_NEW("Entity"); break;
                default: Global::addEntity(new Unknown    (data)); INCR_NEW("Entity"); break;
            }
            break;

        case Global::Levels::Crazy_Gadget:
            switch (objectType)
            {
                case 0:  Global::addEntity(new RING       (data)); INCR_NEW("Entity"); break;
                case 1:  Global::addEntity(new RING_LINEAR(data)); INCR_NEW("Entity"); break;
                case 2:  Global::addEntity(new RING_CIRCLE(data)); INCR_NEW("Entity"); break;
                case 3:  Global::addEntity(new SPRA       (data)); INCR_NEW("Entity"); break;
                case 4:  Global::addEntity(new SPRB       (data)); INCR_NEW("Entity"); break;
                default: Global::addEntity(new Unknown    (data)); INCR_NEW("Entity"); break;
            }
            break;

        case Global::Levels::Cosmic_Wall:
            switch (objectType)
            {
                case 0:  Global::addEntity(new RING       (data)); INCR_NEW("Entity"); break;
                case 1:  Global::addEntity(new RING_LINEAR(data)); INCR_NEW("Entity"); break;
                case 2:  Global::addEntity(new RING_CIRCLE(data)); INCR_NEW("Entity"); break;
                case 3:  Global::addEntity(new SPRA       (data)); INCR_NEW("Entity"); break;
                case 4:  Global::addEntity(new SPRB       (data)); INCR_NEW("Entity"); break;
                default: Global::addEntity(new Unknown    (data)); INCR_NEW("Entity"); break;
            }
            break;

        default: break;
    }

}

void LevelLoader::loadLevelData()
{
    //Global::levelFileMap.clear();
    //Global::levelFileMap["Green Forest"] = "GreenForest.lvl2";
    //Global::levelFileMap["Metal Harbor"] = "MetalHarbor.lvl2";
    //Global::levelFileMap["Sky Rail"]     = "SkyRail.lvl2";
    //Global::levelFileMap["Final Rush"]   = "FinalRush.lvl2";
    //Global::levelFileMap["Crazy Gadget"] = "CrazyGadget.lvl2";

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
    Global::levelSetToLVL2["set0008_2p_s.bin"]     = "";
    Global::levelSetToLVL2["set0008_2p_u.bin"]     = "";
    Global::levelSetToLVL2["set0008_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0008_s.bin"]        = "";
    Global::levelSetToLVL2["set0008_u.bin"]        = "";
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
    Global::levelSetToLVL2["set0018_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0018_hd_u.bin"]     = "";
    Global::levelSetToLVL2["set0018_s.bin"]        = "";
    Global::levelSetToLVL2["set0018_u.bin"]        = "";
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
    Global::levelSetToLVL2["set0032_2p_s.bin"]     = "";
    Global::levelSetToLVL2["set0032_2p_u.bin"]     = "";
    Global::levelSetToLVL2["set0032_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0032_hd_u.bin"]     = "";
    Global::levelSetToLVL2["set0032_s.bin"]        = "";
    Global::levelSetToLVL2["set0032_u.bin"]        = "";
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
    Global::levelSetToLVL2["set0040_2p_s.bin"]     = "";
    Global::levelSetToLVL2["set0040_2p_u.bin"]     = "";
    Global::levelSetToLVL2["set0040_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0040_hd_u.bin"]     = "";
    Global::levelSetToLVL2["set0040_s.bin"]        = "";
    Global::levelSetToLVL2["set0040_u.bin"]        = "";
    Global::levelSetToLVL2["SET0041_2P_S.bin"]     = "";
    Global::levelSetToLVL2["set0041_2p_u.bin"]     = "";
    Global::levelSetToLVL2["SET0042_S.bin"]        = "";
    Global::levelSetToLVL2["set0042_u.bin"]        = "";
    Global::levelSetToLVL2["set0043_hd_s.bin"]     = "CosmicWall.lvl2";
    Global::levelSetToLVL2["set0043_hd_u.bin"]     = "CosmicWall.lvl2";
    Global::levelSetToLVL2["set0043_s.bin"]        = "CosmicWall.lvl2";
    Global::levelSetToLVL2["set0043_u.bin"]        = "CosmicWall.lvl2";
    Global::levelSetToLVL2["set0044_2p_s.bin"]     = "";
    Global::levelSetToLVL2["set0044_2p_u.bin"]     = "";
    Global::levelSetToLVL2["set0044_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0044_hd_u.bin"]     = "";
    Global::levelSetToLVL2["set0044_s.bin"]        = "";
    Global::levelSetToLVL2["set0044_u.bin"]        = "";
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

    printf("'%s'\n", folder.c_str());
    printf("'%s'\n", file1.c_str());
    printf("'%s'\n", file2.c_str());
    printf("\n");

    //find which one is u
    char lastChr1 = file1[file1.size() - 5];
    char lastChr2 = file2[file2.size() - 5];

    printf("'%c'\n", lastChr1);
    printf("'%c'\n", lastChr2);
    printf("\n");

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

    //write the U file as blank
    std::ofstream myfileU((folder+"\\")+fileU, std::ios::out | std::ios::binary);
    if (!myfileU.is_open())
    {
        std::string exp = (folder+"\\")+fileU;
        MessageBox(NULL, (("Cannot open file '"+exp)+"' for writing.").c_str(), "silly", NULL);
        return;
    }
    char buf[32] = {0};
    buf[3] = 1;
    myfileU.write(buf, 32);
    buf[3] = 0;
    myfileU.write(buf, 32); //put a single ring, to avoid crashing
    myfileU.close();

    //write the S file as blank
    std::ofstream myfileS((folder+"\\")+fileS, std::ios::out | std::ios::binary);
    if (!myfileS.is_open())
    {
        std::string exp = (folder+"\\")+fileS;
        MessageBox(NULL, (("Cannot open file '"+exp)+"' for writing.").c_str(), "silly", NULL);
        return;
    }

    memset(buf, 0, 32);
    int count = 0;
    for (Entity* e : Global::gameEntities)
    {
        printf("%d\n", (int)e->isSA2Object());
        if (e->isSA2Object())
        {
            count++;
        }
    }
    buf[3] = (count >>  0) & 0xFF;
    buf[2] = (count >>  8) & 0xFF;
    buf[1] = (count >> 16) & 0xFF;
    buf[0] = (count >> 24) & 0xFF;
    myfileS.write(buf, 32);

    memset(buf, 0, 32);
    for (Entity* e : Global::gameEntities)
    {
        if (e->isSA2Object())
        {
            if (SA2Object* o = dynamic_cast<SA2Object*>(e))
            {
                memset(buf, 0, 32);
                o->fillData(buf);
                myfileS.write(buf, 32);
            }
            else
            {
                std::fprintf(stdout, "Warning: object lied about being an sa2object\n");
            }
        }
    }

    myfileS.close();
}