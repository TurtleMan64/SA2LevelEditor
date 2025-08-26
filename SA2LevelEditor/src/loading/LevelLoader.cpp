#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <list>
#include <cctype>

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
#include "../entities/GlobalObjects/lightsw.h"
#include "../entities/GlobalObjects/rocket.h"
#include "../entities/GlobalObjects/linklink.h"
#include "../entities/GlobalObjects/stoplockon.h"
#include "../entities/GlobalObjects/bunchin.h"
#include "../entities/GlobalObjects/switch.h"
#include "../entities/GlobalObjects/ironball2.h"
#include "../entities/GlobalObjects/swdrngl.h"
#include "../entities/GlobalObjects/swdrngc.h"
#include "../entities/GlobalObjects/kddrngl.h"
#include "../entities/GlobalObjects/kddrngc.h"
#include "../entities/GlobalObjects/kdspring.h"
#include "../entities/GlobalObjects/kdspringb.h"
#include "../entities/GlobalObjects/sgring.h"
#include "../entities/GlobalObjects/sgspra.h"
#include "../entities/GlobalObjects/sgsprb.h"
#include "../entities/GlobalObjects/knudai.h"
#include "../entities/GlobalObjects/emeraldf.h"
#include "../entities/GlobalObjects/itembox.h"
#include "../entities/GlobalObjects/itemboxair.h"
#include "../entities/GlobalObjects/itemboxballoon.h"
#include "../entities/GlobalObjects/savepoint.h"
#include "../entities/GlobalObjects/3spring.h"
#include "../entities/LevelSpecific/PyramidCave/spiderweb.h"
#include "../entities/GlobalObjects/ekumi.h"
#include "../toolbox/maths.h"
#include "../entities/GlobalObjects/eai.h"
#include "../entities/cameratrigger.h"
#include "../entities/loopspeedtrigger.h"
#include "../entities/GlobalObjects/Heroes/dashring.h"
#include "../entities/GlobalObjects/Heroes/pawn.h"
#include "../entities/GlobalObjects/Heroes/searcher.h"
#include "../entities/GlobalObjects/schbox.h"
#include "../entities/GlobalObjects/Heroes/pawngun.h"
#include "../entities/GlobalObjects/esaru.h"
#include "../entities/GlobalObjects/Badniks/motobug.h"
#include "../entities/GlobalObjects/Badniks/rhinotank.h"
#include "../entities/GlobalObjects/Badniks/spinner.h"
#include "../entities/GlobalObjects/soapsw.h"
#include "../entities/GlobalObjects/tjumpdai.h"
#include "../entities/LevelSpecific/PyramidCave/torchcup.h"
#include "../entities/LevelSpecific/PyramidCave/snakestatue.h"
#include "../entities/LevelSpecific/PyramidCave/sneakrail.h"
#include "../entities/LevelSpecific/CosmicWall/cw_stage.h"

#include <Windows.h>
#include <commdlg.h>
#include <tchar.h>

int LevelLoader::numLevels = 0;
std::vector<std::string> LevelLoader::lvlFile;
std::unordered_map<int, std::string> LevelLoader::objectIdToName;

void LevelLoader::initObjectMap()
{
    //go through all the files and load the values into the map
    std::string folder = "res/StageObjectLists/";

    for (int lvlId = 0; lvlId <= 70; lvlId++)
    {
        std::string fname = folder + std::to_string(lvlId) + ".txt";
        std::ifstream file(fname);
        if (!file.is_open())
        {
            continue;
        }

        std::string line;
        int objId = 0;
        while (!file.eof())
        {
            getlineSafe(file, line);
            if (line.size() > 0)
            {
                int key = (lvlId << 8) | objId;
                LevelLoader::objectIdToName[key] = line;
            }
            objId++;
        }
        file.close();
    }
}

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
    //switch (Global::levelID)
    //{
    //    case Global::Levels::Pyramid_Cave:
    //        SPIDERWEB::loadStaticModels();
    //        TORCHCUP::loadStaticModels();
    //        SNAKESTATUE::loadStaticModels();
    //        SNEAKRAIL::loadStaticModels();
    //        break;
    //
    //    default:
    //        break;
    //}

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

    //now go through the camera file
    std::string camFilename = "res/Camera/stg" + std::to_string(Global::levelID) + "cam.txt";
    std::ifstream camFile(camFilename);
    if (!camFile.is_open())
    {
        std::fprintf(stdout, "Error: Cannot load camera file '%s'\n", (camFilename).c_str());
        camFile.close();
    }
    else
    {
        while (!camFile.eof())
        {
            getlineSafe(camFile, line);

            char lineBuf[512]; //Buffer to copy line into
            memcpy(lineBuf, line.c_str(), line.size()+1);

            int splitLength = 0;
            char** lineSplit = split(lineBuf, ' ', &splitLength);

            if (splitLength > 0)
            {
                processCameraTrigger(lineSplit, splitLength);
            }
            free(lineSplit);
        }
        camFile.close();
    }

    //now go through the loopspeed file
    std::string loopFilename = "res/Loops/stg" + std::to_string(Global::levelID) + "loop.txt";
    std::ifstream loopFile(loopFilename);
    if (!loopFile.is_open())
    {
        std::fprintf(stdout, "Error: Cannot load loop file '%s'\n", (loopFilename).c_str());
        loopFile.close();
    }
    else
    {
        while (!loopFile.eof())
        {
            getlineSafe(loopFile, line);

            char lineBuf[512]; //Buffer to copy line into
            memcpy(lineBuf, line.c_str(), line.size()+1);

            int splitLength = 0;
            char** lineSplit = split(lineBuf, ' ', &splitLength);

            if (splitLength > 0)
            {
                processLoopspeedTrigger(lineSplit, splitLength);
            }
            free(lineSplit);
        }
        loopFile.close();
    }

    //Loader::printInfo();

    Global::gameState = STATE_RUNNING;

    glfwSetTime(0);
    extern double timeOld;
    //extern double previousTime;
    timeOld = 0.0;
    //previousTime = 0.0;
}

void LevelLoader::loadLevelSAB(std::string fileLvl, std::string fileObj)
{
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

    LevelLoader::lvlFile.clear();

    Global::levelID = Global::Custom_Level;

    int idx  = (int)fileObj.find_last_of('\\');
    int idx2 = (int)fileObj.find_last_of('.');
    std::string objDir = fileObj.substr(0, idx+1);
    std::string objName = fileObj.substr(idx+1, (idx2-idx)-1);

    //printf("objDir  = '%s'\n", objDir.c_str());
    //printf("objName = '%s'\n", objName.c_str());

    Stage::loadModels(objDir.c_str(), objName.c_str());

    std::ifstream file(fileLvl);
    if (!file.is_open())
    {
        std::fprintf(stdout, "Error: Cannot load file '%s'\n", (fileLvl).c_str());
        file.close();
        return;
    }

    std::string line;
    bool objectsStarted = false;

    while (!file.eof())
    {
        getlineSafe(file, line);
        LevelLoader::lvlFile.push_back(line);

        char lineBuf[512]; //Buffer to copy line into
        memcpy(lineBuf, line.c_str(), line.size()+1);

        int splitLength = 0;
        char** lineSplit = split(lineBuf, ' ', &splitLength);

        if (splitLength == 0)
        {
            objectsStarted = true; //header is done
        }
        else if (objectsStarted && splitLength > 0)
        {
            processLine(lineSplit, splitLength);
        }
        free(lineSplit);
    }
    file.close();
}

void fillFloat(float value, char* buf)
{
    char* ptr = (char*)(&value);
    buf[0] = (char)(*(ptr + 3));
    buf[1] = (char)(*(ptr + 2));
    buf[2] = (char)(*(ptr + 1));
    buf[3] = (char)(*(ptr + 0));
}

void fillShort(int value, char* buf)
{
    buf[0] = (char)((value >> 8) & 0xFF);
    buf[1] = (char)((value >> 0) & 0xFF);
}

void LevelLoader::processLine(char** dat, int datLength)
{
    if (dat[0][0] == '#')
    {
        return;
    }

    int id = std::stoi(dat[0]);

    char buf[32] = {0};
    buf[1] = (char)id;

    switch (id)
    {
        case 0: //Ring
        {
            fillFloat(toFloat(dat[1]), &buf[8]);
            fillFloat(toFloat(dat[2]), &buf[12]);
            fillFloat(toFloat(dat[3]), &buf[16]);
            RING* ring = new RING(buf, false); INCR_NEW("Entity");
            ring->lvlLineNum = (int)LevelLoader::lvlFile.size()-1;
            Global::addEntity(ring);
            return;
        }

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

        case 8: //Boostpad
        {
            //fill in to buffer
            fillShort(Maths::degToBams(toFloat(dat[6])), &buf[ 2]); //rotations
            fillShort(Maths::degToBams(toFloat(dat[7])), &buf[ 4]);
            fillShort(Maths::degToBams(toFloat(dat[8])), &buf[ 6]);
            fillFloat(toFloat(dat[1]),                   &buf[ 8]); //position
            fillFloat(toFloat(dat[2]),                   &buf[12]);
            fillFloat(toFloat(dat[3]),                   &buf[16]);
            fillFloat(toFloat(dat[4])/60,                &buf[20]); //vars
            fillFloat(toFloat(dat[5])*60,                &buf[24]);
            if (datLength >= 10)
            {
                fillFloat(toFloat(dat[9]),                   &buf[28]);
            }

            KASOKU* pad = new KASOKU(buf, false); INCR_NEW("Entity");
            pad->lvlLineNum = (int)LevelLoader::lvlFile.size()-1;
            Global::addEntity(pad);
            return;
        }

        case 10: //Checkpoint
        {
            int yrotBam = Maths::degToBams(toFloat(dat[4])+90);

            //fill in to buffer
            fillShort(yrotBam,         &buf[ 4]); //rotation
            fillFloat(toFloat(dat[1]), &buf[ 8]); //position
            fillFloat(toFloat(dat[2]), &buf[12]);
            fillFloat(toFloat(dat[3]), &buf[16]);

            SAVEPOINT* check = new SAVEPOINT(buf, false); INCR_NEW("Entity");
            check->lvlLineNum = (int)LevelLoader::lvlFile.size() - 1;
            Global::addEntity(check);
            return;
        }

        case 11: //JumpPad
        {
            Vector3f forward(toFloat(dat[4]), toFloat(dat[5]), toFloat(dat[6]));
            forward.normalize();

            float rotX = Maths::toDegrees(atan2f(forward.y, sqrtf(forward.x*forward.x + forward.z*forward.z)));
            float rotY = Maths::toDegrees(atan2f(-forward.x, -forward.z));

            //fill in to buffer
            fillShort(Maths::degToBams(rotX), &buf[ 2]); //rotations
            fillShort(Maths::degToBams(rotY), &buf[ 4]);
            fillShort(Maths::degToBams(0),    &buf[ 6]);
            fillFloat(toFloat(dat[1]),        &buf[ 8]); //position
            fillFloat(toFloat(dat[2]),        &buf[12]);
            fillFloat(toFloat(dat[3]),        &buf[16]);
            fillFloat(toFloat(dat[7])/60,     &buf[20]); //vars
            fillFloat(toFloat(dat[8])*60,     &buf[24]);

            BIGJUMP* ramp = new BIGJUMP(buf, false); INCR_NEW("Entity");
            ramp->lvlLineNum = (int)LevelLoader::lvlFile.size()-1;
            Global::addEntity(ramp);
            return;
        }

        case 12: //Spring
        {
            //calculation rotations in XZ order
            Vector3f zAxis(0, 0, 1);

            Vector3f dir(toFloat(dat[4]), toFloat(dat[5]), toFloat(dat[6]));
            dir.normalize();

            float zrot = atan2f(dir.x, dir.y);
            dir = Maths::rotatePoint(&dir, &zAxis, zrot);
            float xrot = atan2f(dir.z, dir.y);

            int zrotBam = Maths::radToBams(-zrot);
            int xrotBam = Maths::radToBams(xrot);

            //fill in to buffer
            fillShort(xrotBam, &buf[2]); //rotations
            fillShort(zrotBam, &buf[6]);
            fillFloat(toFloat(dat[1]), &buf[8]);  //position
            fillFloat(toFloat(dat[2]), &buf[12]);
            fillFloat(toFloat(dat[3]), &buf[16]);
            fillFloat(toFloat(dat[8])*60,     &buf[20]); //vars
            fillFloat((toFloat(dat[7])/60)-5, &buf[24]);
            fillFloat(toFloat(dat[9]),        &buf[28]);

            SPRB* spring = new SPRB(buf, false); INCR_NEW("Entity");
            spring->lvlLineNum = (int)LevelLoader::lvlFile.size()-1;
            Global::addEntity(spring);
            return;
        }

        case 13: //SpringTriple
        {
            Vector3f dir(toFloat(dat[4]), 0, toFloat(dat[5]));
            float yrot = atan2f(-dir.z, dir.x) + Maths::PI/2;

            int yrotBam = Maths::radToBams(yrot);

            //fill in to buffer
            fillShort(0,       &buf[2]); //rotations
            fillShort(yrotBam, &buf[4]);
            fillShort(0,       &buf[6]);
            fillFloat(toFloat(dat[1]), &buf[8]);  //position
            fillFloat(toFloat(dat[2]), &buf[12]);
            fillFloat(toFloat(dat[3]), &buf[16]);
            fillFloat((toFloat(dat[6])/60)-5, &buf[20]); //power
            fillFloat(toFloat(dat[7])*60, &buf[24]); //time

            THREESPRING* spring = new THREESPRING(buf, false); INCR_NEW("Entity");
            spring->lvlLineNum = (int)LevelLoader::lvlFile.size()-1;
            Global::addEntity(spring);
            return;
        }

        case 27: //Item Capsule
        {
            int boxType = toInt(dat[8]);
            float itemType = toFloat(dat[7]);

            //calculation rotations in YZ order
            //Vector3f yAxis(0, 1, 0);

            Vector3f dir(toFloat(dat[4]), toFloat(dat[5]), toFloat(dat[6]));
            dir.normalize();

            //float yrot = atan2f(-dir.z, dir.x);
            //dir = Maths::rotatePoint(&dir, &yAxis, -yrot);
            //float zrot = atan2f(dir.y, dir.x);
            float yrot = atan2f(-dir.z, dir.x);
            float distH = sqrtf(dir.z*dir.z + dir.x*dir.x);
            float zrot = atan2f(dir.y, distH);

            int zrotBam = Maths::radToBams(zrot - Maths::PI/2);
            int yrotBam = Maths::radToBams(yrot);

            //fill in to buffer
            fillShort(yrotBam,         &buf[ 4]); //rotations
            fillShort(zrotBam,         &buf[ 6]);
            fillFloat(toFloat(dat[1]), &buf[ 8]); //position
            fillFloat(toFloat(dat[2]), &buf[12]);
            fillFloat(toFloat(dat[3]), &buf[16]);
            fillFloat(itemType,        &buf[20]); //vars

            if (boxType == 0)
            {
                ITEMBOX* item = new ITEMBOX(buf, false); INCR_NEW("Entity");
                item->lvlLineNum = (int)LevelLoader::lvlFile.size()-1;
                Global::addEntity(item);
            }
            else
            {
                ITEMBOXAIR* item = new ITEMBOXAIR(buf, false); INCR_NEW("Entity");
                item->lvlLineNum = (int)LevelLoader::lvlFile.size()-1;
                Global::addEntity(item);
            }
            return;
        }

        case 28: //Beetle
        {
            fillFloat(toFloat(dat[1]), &buf[ 8]);
            fillFloat(toFloat(dat[2]), &buf[12]);
            fillFloat(toFloat(dat[3]), &buf[16]);
            E_KUMI* beetle = new E_KUMI(buf, false); INCR_NEW("Entity");
            beetle->lvlLineNum = (int)LevelLoader::lvlFile.size()-1;
            Global::addEntity(beetle);
            return;
        }

        case 30: //Hunter
        {
            float yrot = atan2f(-toFloat(dat[5]), toFloat(dat[4]));

            fillShort(Maths::radToBams(yrot), &buf[4]);
            fillFloat(toFloat(dat[1]), &buf[8]);
            fillFloat(toFloat(dat[2]), &buf[12]);
            fillFloat(toFloat(dat[3]), &buf[16]);
            E_AI* hunter = new E_AI(buf, false); INCR_NEW("Entity");
            hunter->lvlLineNum = (int)LevelLoader::lvlFile.size()-1;
            Global::addEntity(hunter);
            return;
        }

        case 73: //Emerald
        {
            fillShort(toInt  (dat[4]), &buf[ 2]);
            fillShort(toInt  (dat[5]), &buf[ 4]);
            fillShort(toInt  (dat[6]), &buf[ 6]);
            fillFloat(toFloat(dat[1]), &buf[ 8]);
            fillFloat(toFloat(dat[2]), &buf[12]);
            fillFloat(toFloat(dat[3]), &buf[16]);
            EMERALD* emerald = new EMERALD(buf, false); INCR_NEW("Entity");
            emerald->lvlLineNum = (int)LevelLoader::lvlFile.size()-1;
            Global::addEntity(emerald);
            return;
        }

        case 75: //Hint Monitor
        {
            fillShort(Maths::degToBams(toFloat(dat[4])), &buf[4]); //yrot
            fillFloat(toFloat(dat[1]), &buf[ 8]); //x
            fillFloat(toFloat(dat[2]), &buf[12]); //y
            fillFloat(toFloat(dat[3]), &buf[16]); //z
            SCHBOX* monitor = new SCHBOX(buf, false); INCR_NEW("Entity");
            monitor->lvlLineNum = (int)LevelLoader::lvlFile.size()-1;
            Global::addEntity(monitor);
            return;
        }
        
        case 98: //Line of rings
        {
            Vector3f pos1(toFloat(dat[1]), toFloat(dat[2]), toFloat(dat[3]));
            Vector3f pos2(toFloat(dat[4]), toFloat(dat[5]), toFloat(dat[6]));
            Vector3f dir = pos2 - pos1;
            int numRings = toInt(dat[7]);
            float totalDist = dir.length();
            float spacing = totalDist/(numRings-1);

            Vector3f yAxis(0, 1, 0);
            float yRot = atan2f(-dir.x, -dir.z);
            dir = Maths::rotatePoint(&dir, &yAxis, -yRot);
            float xRot = atan2f(dir.y, -dir.z);

            fillShort(Maths::radToBams(xRot), &buf[ 2]);
            fillShort(Maths::radToBams(yRot), &buf[ 4]);
            fillFloat(toFloat(dat[1]),        &buf[ 8]);
            fillFloat(toFloat(dat[2]),        &buf[12]);
            fillFloat(toFloat(dat[3]),        &buf[16]);
            fillFloat(spacing - 10.0f,        &buf[20]);
            fillFloat(0.0f,                   &buf[24]);
            fillFloat((float)numRings,        &buf[28]);
            RING_LINEAR* line = new RING_LINEAR(buf, false); INCR_NEW("Entity");
            line->lvlLineNum = (int)LevelLoader::lvlFile.size()-1;
            Global::addEntity(line);
            return;
        }

        case 99: //Circle of rings
        {
            float radius = toFloat(dat[4]);
            int numRings = toInt(dat[5]);

            fillFloat(toFloat(dat[1]), &buf[ 8]);
            fillFloat(toFloat(dat[2]), &buf[12]);
            fillFloat(toFloat(dat[3]), &buf[16]);
            fillFloat(radius - 10.0f,  &buf[20]);
            fillFloat(0.0f,            &buf[24]);
            fillFloat((float)numRings, &buf[28]);
            RING_CIRCLE* circle = new RING_CIRCLE(buf, false); INCR_NEW("Entity");
            circle->lvlLineNum = (int)LevelLoader::lvlFile.size() - 1;
            Global::addEntity(circle);
            return;
        }

        case 115: //Item Balloon
        {
            //fill in to buffer
            fillFloat(toFloat(dat[1]), &buf[8]);  //position
            fillFloat(toFloat(dat[2]), &buf[12]);
            fillFloat(toFloat(dat[3]), &buf[16]);
            fillFloat(toFloat(dat[4]), &buf[20]); //vars

            ITEMBOXBALLOON* item = new ITEMBOXBALLOON(buf, false); INCR_NEW("Entity");
            item->lvlLineNum = (int)LevelLoader::lvlFile.size()-1;
            Global::addEntity(item);
            return;
        }

        case 117: //Flapper
        {
            float yrot = atan2f(-toFloat(dat[6]), toFloat(dat[5]));

            fillShort(Maths::radToBams(yrot), &buf[ 4]);
            fillFloat(toFloat(dat[1]),        &buf[ 8]);
            fillFloat(toFloat(dat[2]),        &buf[12]);
            fillFloat(toFloat(dat[3]),        &buf[16]);
            fillFloat(toFloat(dat[4]),        &buf[20]);
            SEARCHER* flapper = new SEARCHER(buf, false); INCR_NEW("Entity");
            flapper->lvlLineNum = (int)LevelLoader::lvlFile.size() - 1;
            Global::addEntity(flapper);
            return;
        }

        case 118: //Dash Ring
        {
            Vector3f dir(toFloat(dat[4]), toFloat(dat[5]), toFloat(dat[6]));
            dir.normalize();

            float yrot = atan2f(-dir.z, dir.x);
            float distH = sqrtf(dir.z*dir.z + dir.x*dir.x);
            float zrot = atan2f(dir.y, distH);

            int yrotBam = Maths::radToBams(yrot);
            int zrotBam = Maths::radToBams(zrot);

            int camera  = toInt(dat[9]);
            int rainbow = toInt(dat[10]);

            float power = toFloat(dat[7])/60;
            float controlTime = toFloat(dat[8])*60;

            //fill in to buffer
            fillShort(0,               &buf[ 2]); //rotations
            fillShort(yrotBam,         &buf[ 4]);
            fillShort(zrotBam,         &buf[ 6]);
            fillFloat(toFloat(dat[1]), &buf[ 8]);  //position
            fillFloat(toFloat(dat[2]), &buf[12]);
            fillFloat(toFloat(dat[3]), &buf[16]);
            fillFloat(power,           &buf[20]); //vars
            fillFloat(controlTime,     &buf[24]);
            buf[28] = (char)camera;
            buf[29] = (char)rainbow;

            DASHRING* dash = new DASHRING(buf, false); INCR_NEW("Entity");
            dash->lvlLineNum = (int)LevelLoader::lvlFile.size()-1;
            Global::addEntity(dash);
            return;
        }

        case 119: //Pawn
        {
            float yrot = atan2f(-toFloat(dat[5]), toFloat(dat[4]));

            fillShort(Maths::radToBams(yrot), &buf[ 4]);
            fillFloat(toFloat(dat[1]),        &buf[ 8]);
            fillFloat(toFloat(dat[2]),        &buf[12]);
            fillFloat(toFloat(dat[3]),        &buf[16]);
            PAWN* pawn = new PAWN(buf, false); INCR_NEW("Entity");
            pawn->lvlLineNum = (int)LevelLoader::lvlFile.size() - 1;
            Global::addEntity(pawn);
            return;
        }

        case 120: //Moto Bug
        {
            fillFloat(toFloat(dat[1]),        &buf[ 8]);
            fillFloat(toFloat(dat[2]),        &buf[12]);
            fillFloat(toFloat(dat[3]),        &buf[16]);
            MOTOBUG* pawn = new MOTOBUG(buf, false); INCR_NEW("Entity");
            pawn->lvlLineNum = (int)LevelLoader::lvlFile.size() - 1;
            Global::addEntity(pawn);
            return;
        }

        case 121: //Rhino Tank
        {
            fillFloat(toFloat(dat[1]),        &buf[ 8]);
            fillFloat(toFloat(dat[2]),        &buf[12]);
            fillFloat(toFloat(dat[3]),        &buf[16]);
            RHINOTANK* pawn = new RHINOTANK(buf, false); INCR_NEW("Entity");
            pawn->lvlLineNum = (int)LevelLoader::lvlFile.size() - 1;
            Global::addEntity(pawn);
            return;
        }

        case 123: //Spinner
        {
            fillFloat(toFloat(dat[1]),        &buf[ 8]);
            fillFloat(toFloat(dat[2]),        &buf[12]);
            fillFloat(toFloat(dat[3]),        &buf[16]);
            SPINNER* pawn = new SPINNER(buf, false); INCR_NEW("Entity");
            pawn->lvlLineNum = (int)LevelLoader::lvlFile.size() - 1;
            Global::addEntity(pawn);
            return;
        }

        case 124: //Pawn Gun
        {
            float yrot = atan2f(-toFloat(dat[5]), toFloat(dat[4]));

            fillShort(Maths::radToBams(yrot), &buf[ 4]);
            fillFloat(toFloat(dat[1]),        &buf[ 8]);
            fillFloat(toFloat(dat[2]),        &buf[12]);
            fillFloat(toFloat(dat[3]),        &buf[16]);
            PAWNGUN* pawn = new PAWNGUN(buf, false); INCR_NEW("Entity");
            pawn->lvlLineNum = (int)LevelLoader::lvlFile.size() - 1;
            Global::addEntity(pawn);
            return;
        }

        default:
        {
            return;
        }
    }
}

void LevelLoader::processCameraTrigger(char** dat, int dataLength)
{
    if (dataLength != 16)
    {
        return;
    }

    CameraTrigger* trigger = new CameraTrigger(std::stoi(dat[ 0]), 
                                               std::stof(dat[ 1]), std::stof(dat[ 2]), std::stof(dat[ 3]),
                                               std::stof(dat[ 4]), std::stof(dat[ 5]), std::stof(dat[ 6]),
                                               std::stof(dat[ 7]), std::stof(dat[ 8]), std::stof(dat[ 9]),
                                               std::stof(dat[10]), std::stof(dat[11]), std::stof(dat[12]),
                                               std::stof(dat[13]), std::stof(dat[14]), std::stof(dat[15])); INCR_NEW("Entity");
    
    Global::addTransparentEntity(trigger);
}

void LevelLoader::processLoopspeedTrigger(char** dat, int dataLength)
{
    if (dataLength != 3)
    {
        return;
    }

    LoopspeedTrigger* trigger = new LoopspeedTrigger(std::stof(dat[ 0]), std::stof(dat[ 1]), std::stof(dat[ 2])); INCR_NEW("Entity");
    
    Global::addTransparentEntity(trigger);
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

    if (levelID == Global::Levels::Custom_Level)
    {
        switch (objectID)
        {
            case    0: return new RING          (data, useDefaultValues);
            case   98: return new RING_LINEAR   (data, useDefaultValues);
            case   99: return new RING_CIRCLE   (data, useDefaultValues);
            case   12: return new SPRB          (data, useDefaultValues);
            case   13: return new THREESPRING   (data, useDefaultValues);
            case   11: return new BIGJUMP       (data, useDefaultValues);
            case    8: return new KASOKU        (data, useDefaultValues);
            case   10: return new SAVEPOINT     (data, useDefaultValues);
            case   27: return new ITEMBOX       (data, useDefaultValues);
            case   28: return new E_KUMI        (data, useDefaultValues);
            case   30: return new E_AI          (data, useDefaultValues);
            //case   97: return new ROCKET        (data, useDefaultValues);
            case  115: return new ITEMBOXBALLOON(data, useDefaultValues);
            case  117: return new SEARCHER      (data, useDefaultValues);
            case  118: return new DASHRING      (data, useDefaultValues);
            case  119: return new PAWN          (data, useDefaultValues);
            case   73: return new EMERALD       (data, useDefaultValues); 
            case   75: return new SCHBOX        (data, useDefaultValues); 
            default:   return new RING          (data, useDefaultValues);
        }
    }

    int key = (levelID << 8) | objectID;
    if (LevelLoader::objectIdToName.find(key) == LevelLoader::objectIdToName.end())
    {
        return new Unknown(data, useDefaultValues);
    }

    std::string o = LevelLoader::objectIdToName[key];
    if      (o == "RING")           {return new RING          (data, useDefaultValues);}
    else if (o == "RING_LINEAR")    {return new RING_LINEAR   (data, useDefaultValues);}
    else if (o == "RING_CIRCLE")    {return new RING_CIRCLE   (data, useDefaultValues);}
    else if (o == "SPRA")           {return new SPRA          (data, useDefaultValues);}
    else if (o == "SPRB")           {return new SPRB          (data, useDefaultValues);}
    else if (o == "3SPRING")        {return new THREESPRING   (data, useDefaultValues);}
    else if (o == "BIGJUMP")        {return new BIGJUMP       (data, useDefaultValues);}
    else if (o == "KASOKU")         {return new KASOKU        (data, useDefaultValues);}
    else if (o == "SAVEPOINT")      {return new SAVEPOINT     (data, useDefaultValues);}
    else if (o == "SWITCH")         {return new SWITCH        (data, useDefaultValues);}
    else if (o == "ITEMBOX")        {return new ITEMBOX       (data, useDefaultValues);}
    else if (o == "ITEMBOXAIR")     {return new ITEMBOXAIR    (data, useDefaultValues);}
    else if (o == "ITEMBOXBALLOON") {return new ITEMBOXBALLOON(data, useDefaultValues);}
    else if (o == "ROCKET")         {return new ROCKET        (data, useDefaultValues);}
    else if (o == "KNUDAI")         {return new KNUDAI        (data, useDefaultValues);}
    else if (o == "KDDRNGL")        {return new KDDRNGL       (data, useDefaultValues);}
    else if (o == "KDDRNGC")        {return new KDDRNGC       (data, useDefaultValues);}
    else if (o == "KDSPRING")       {return new KDSPRING      (data, useDefaultValues);}
    else if (o == "KDSPRINGB")      {return new KDSPRINGB     (data, useDefaultValues);}
    else if (o == "SPHERE")         {return new SPHERE        (data, useDefaultValues);}
    else if (o == "CCYL")           {return new CCYL          (data, useDefaultValues);}
    else if (o == "CCUBE")          {return new CCUBE         (data, useDefaultValues);}
    else if (o == "BUNCHIN")        {return new BUNCHIN       (data, useDefaultValues);}
    else if (o == "IRONBALL2")      {return new IRONBALL2     (data, useDefaultValues);}
    else if (o == "E KUMI")         {return new E_KUMI        (data, useDefaultValues);}
    else if (o == "LIGHT SW")       {return new LIGHT_SW      (data, useDefaultValues);}
    else if (o == "SWDRNGL")        {return new SWDRNGL       (data, useDefaultValues);}
    else if (o == "SWDRNGC")        {return new SWDRNGC       (data, useDefaultValues);}
    else if (o == "LINKLINK")       {return new LINKLINK      (data, useDefaultValues);}
    else if (o == "SOAP SW")        {return new SOAP_SW       (data, useDefaultValues);}
    else if (o == "STOPLOCKON")     {return new STOPLOCKON    (data, useDefaultValues);}
    else if (o == "SG RING")        {return new SG_RING       (data, useDefaultValues);}
    else if (o == "SG SPRA")        {return new SG_SPRA       (data, useDefaultValues);}
    else if (o == "SG SPRB")        {return new SG_SPRB       (data, useDefaultValues);}
    else if (o == "EMERALD")        {return new EMERALD       (data, useDefaultValues);}
    else if (o == "EMERALD F")      {return new EMERALD_F     (data, useDefaultValues);}
    else if (o == "SPIDERWEB")      {return new SPIDERWEB     (data, useDefaultValues);}
    else if (o == "TORCHCUP")       {return new TORCHCUP      (data, useDefaultValues);}
    else if (o == "SNAKESTATUE")    {return new SNAKESTATUE   (data, useDefaultValues);}
    else if (o == "SNEAKRAIL")      {return new SNEAKRAIL     (data, useDefaultValues);}
    else if (o == "SCHBOX")         {return new SCHBOX        (data, useDefaultValues);}
    else if (o == "TJUMPDAI")       {return new TJUMPDAI      (data, useDefaultValues);}
    else if (o == "STAGE")          {return new CW_STAGE      (data, useDefaultValues);}
    else                            {return new Unknown       (data, useDefaultValues);}
}

void LevelLoader::loadLevelData()
{
    LevelLoader::initObjectMap();

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
    Global::levelSetToLVL2["SET0000_S.BIN"]        = "BasicTest.lvl2";
    Global::levelSetToLVL2["SET0000_U.BIN"]        = "BasicTest.lvl2";
    Global::levelSetToLVL2["set0001_s.bin"]        = "";
    Global::levelSetToLVL2["set0001_u.bin"]        = "";
    Global::levelSetToLVL2["set0003_2p_s.bin"]     = "";
    Global::levelSetToLVL2["set0003_2p_u.bin"]     = "";
    Global::levelSetToLVL2["set0003_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0003_hd_u.bin"]     = "";
    Global::levelSetToLVL2["set0003_s.bin"]        = "GreenForest.lvl2";
    Global::levelSetToLVL2["set0003_u.bin"]        = "GreenForest.lvl2";
    Global::levelSetToLVL2["set0004_2p_s.bin"]     = "";
    Global::levelSetToLVL2["set0004_2p_u.bin"]     = "";
    Global::levelSetToLVL2["set0004_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0004_s.bin"]        = "WhiteJungle.lvl2";
    Global::levelSetToLVL2["set0004_u.bin"]        = "WhiteJungle.lvl2";
    Global::levelSetToLVL2["set0005_2P_s.bin"]     = "";
    Global::levelSetToLVL2["set0005_2P_u.bin"]     = "";
    Global::levelSetToLVL2["set0005_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0005_s.bin"]        = "PumpkinHill.lvl2";
    Global::levelSetToLVL2["set0005_u.bin"]        = "PumpkinHill.lvl2";
    Global::levelSetToLVL2["set0006_2p_s.bin"]     = "";
    Global::levelSetToLVL2["set0006_2p_u.bin"]     = "";
    Global::levelSetToLVL2["set0006_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0006_hd_u.bin"]     = "";
    Global::levelSetToLVL2["set0006_s.bin"]        = "SkyRail.lvl2";
    Global::levelSetToLVL2["set0006_u.bin"]        = "SkyRail.lvl2";
    Global::levelSetToLVL2["set0007_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0007_hd_u.bin"]     = "";
    Global::levelSetToLVL2["set0007_s.bin"]        = "AquaticMine.lvl2";
    Global::levelSetToLVL2["set0007_u.bin"]        = "AquaticMine.lvl2";
    Global::levelSetToLVL2["set0008_2p_s.bin"]     = "";
    Global::levelSetToLVL2["set0008_2p_u.bin"]     = "";
    Global::levelSetToLVL2["set0008_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0008_s.bin"]        = "SecurityHall.lvl2";
    Global::levelSetToLVL2["set0008_u.bin"]        = "SecurityHall.lvl2";
    Global::levelSetToLVL2["set0009_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0009_s.bin"]        = "PrisonLane.lvl2";
    Global::levelSetToLVL2["set0009_u.bin"]        = "PrisonLane.lvl2";
    Global::levelSetToLVL2["set0010_2p_u.bin"]     = "";
    Global::levelSetToLVL2["set0010_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0010_hd_u.bin"]     = "";
    Global::levelSetToLVL2["set0010_s.bin"]        = "MetalHarbor.lvl2";
    Global::levelSetToLVL2["set0010_u.bin"]        = "MetalHarbor.lvl2";
    Global::levelSetToLVL2["set0011_2p_s.bin"]     = "";
    Global::levelSetToLVL2["set0011_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0011_hd_u.bin"]     = "";
    Global::levelSetToLVL2["set0011_s.bin"]        = "IronGate.lvl2";
    Global::levelSetToLVL2["set0011_u.bin"]        = "IronGate.lvl2";
    Global::levelSetToLVL2["set0012_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0012_hd_u.bin"]     = "";
    Global::levelSetToLVL2["set0012_s.bin"]        = "WeaponsBed.lvl2";
    Global::levelSetToLVL2["set0012_u.bin"]        = "WeaponsBed.lvl2";
    Global::levelSetToLVL2["set0013_2p_s.bin"]     = "";
    Global::levelSetToLVL2["set0013_2p_u.bin"]     = "";
    Global::levelSetToLVL2["set0013_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0013_s.bin"]        = "CityEscape.lvl2";
    Global::levelSetToLVL2["set0013_u.bin"]        = "CityEscape.lvl2";
    Global::levelSetToLVL2["set0014_2p_s.bin"]     = "";
    Global::levelSetToLVL2["set0014_2p_u.bin"]     = "";
    Global::levelSetToLVL2["set0014_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0014_s.bin"]        = "RadicalHighway.lvl2";
    Global::levelSetToLVL2["set0014_u.bin"]        = "RadicalHighway.lvl2";
    Global::levelSetToLVL2["set0015_s.bin"]        = "WeaponsBed2P.lvl2";
    Global::levelSetToLVL2["set0015_u.bin"]        = "WeaponsBed2P.lvl2";
    Global::levelSetToLVL2["set0016_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0016_hd_u.bin"]     = "";
    Global::levelSetToLVL2["set0016_s.bin"]        = "WildCanyon.lvl2";
    Global::levelSetToLVL2["set0016_u.bin"]        = "WildCanyon.lvl2";
    Global::levelSetToLVL2["set0017_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0017_s.bin"]        = "MissionStreet.lvl2";
    Global::levelSetToLVL2["set0017_u.bin"]        = "MissionStreet.lvl2";
    Global::levelSetToLVL2["set0018_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0018_hd_u.bin"]     = "";
    Global::levelSetToLVL2["set0018_s.bin"]        = "DryLagoon.lvl2";
    Global::levelSetToLVL2["set0018_u.bin"]        = "DryLagoon.lvl2";
    Global::levelSetToLVL2["set0019_s.bin"]        = "SonicVsShadow1.lvl2";
    Global::levelSetToLVL2["SET0019_U.BIN"]        = "SonicVsShadow1.lvl2";
    Global::levelSetToLVL2["SET0020_S.BIN"]        = "TailsVsEggman1.lvl2";
    Global::levelSetToLVL2["SET0020_U.BIN"]        = "TailsVsEggman1.lvl2";
    Global::levelSetToLVL2["set0021_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0021_hd_u.bin"]     = "";
    Global::levelSetToLVL2["set0021_s.bin"]        = "SandOcean.lvl2";
    Global::levelSetToLVL2["set0021_u.bin"]        = "SandOcean.lvl2";
    Global::levelSetToLVL2["set0022_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0022_s.bin"]        = "CrazyGadget.lvl2";
    Global::levelSetToLVL2["set0022_u.bin"]        = "CrazyGadget.lvl2";
    Global::levelSetToLVL2["set0023_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0023_hd_u.bin"]     = "";
    Global::levelSetToLVL2["set0023_s.bin"]        = "HiddenBase.lvl2";
    Global::levelSetToLVL2["set0023_u.bin"]        = "HiddenBase.lvl2";
    Global::levelSetToLVL2["set0024_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0024_s.bin"]        = "EternalEngine.lvl2";
    Global::levelSetToLVL2["SET0024_U.BIN"]        = "EternalEngine.lvl2";
    Global::levelSetToLVL2["set0025_2p_s.bin"]     = "";
    Global::levelSetToLVL2["set0025_2p_u.bin"]     = "";
    Global::levelSetToLVL2["set0025_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0025_s.bin"]        = "DeathChamber.lvl2";
    Global::levelSetToLVL2["set0025_u.bin"]        = "DeathChamber.lvl2";
    Global::levelSetToLVL2["set0026_2p_s.bin"]     = "";
    Global::levelSetToLVL2["set0026_2p_u.bin"]     = "";
    Global::levelSetToLVL2["set0026_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0026_s.bin"]        = "EggQuarters.lvl2";
    Global::levelSetToLVL2["set0026_u.bin"]        = "EggQuarters.lvl2";
    Global::levelSetToLVL2["set0027_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0027_s.bin"]        = "LostColony.lvl2";
    Global::levelSetToLVL2["set0027_u.bin"]        = "LostColony.lvl2";
    Global::levelSetToLVL2["set0028_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0028_hd_u.bin"]     = "";
    Global::levelSetToLVL2["set0028_s.bin"]        = "PyramidCave.lvl2";
    Global::levelSetToLVL2["set0028_u.bin"]        = "PyramidCave.lvl2";
    Global::levelSetToLVL2["set0029_s.bin"]        = "TailsVsEggman2.lvl2";
    Global::levelSetToLVL2["set0029_u.bin"]        = "TailsVsEggman2.lvl2";
    Global::levelSetToLVL2["set0030_2p_s.bin"]     = "";
    Global::levelSetToLVL2["set0030_2p_u.bin"]     = "";
    Global::levelSetToLVL2["set0030_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0030_hd_u.bin"]     = "";
    Global::levelSetToLVL2["set0030_s.bin"]        = "FinalRush.lvl2";
    Global::levelSetToLVL2["set0030_u.bin"]        = "FinalRush.lvl2";
    Global::levelSetToLVL2["set0031_s.bin"]        = "GreenHill.lvl2";
    Global::levelSetToLVL2["set0031_u.bin"]        = "GreenHill.lvl2";
    Global::levelSetToLVL2["set0032_2p_s.bin"]     = "MeteorHerd.lvl2";
    Global::levelSetToLVL2["set0032_2p_u.bin"]     = "MeteorHerd.lvl2";
    Global::levelSetToLVL2["set0032_hd_s.bin"]     = "MeteorHerd.lvl2";
    Global::levelSetToLVL2["set0032_hd_u.bin"]     = "MeteorHerd.lvl2";
    Global::levelSetToLVL2["set0032_s.bin"]        = "MeteorHerd.lvl2";
    Global::levelSetToLVL2["set0032_u.bin"]        = "MeteorHerd.lvl2";
    Global::levelSetToLVL2["set0033_s.bin"]        = "KnucklesVsRouge.lvl2";
    Global::levelSetToLVL2["set0033_u.bin"]        = "KnucklesVsRouge.lvl2";
    Global::levelSetToLVL2["set0034_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0034_s.bin"]        = "CannonsCoreSonic.lvl2";
    Global::levelSetToLVL2["set0034_u.bin"]        = "CannonsCoreSonic.lvl2";
    Global::levelSetToLVL2["set0035_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0035_s.bin"]        = "CannonsCoreEggman.lvl2";
    Global::levelSetToLVL2["set0035_u.bin"]        = "CannonsCoreEggman.lvl2";
    Global::levelSetToLVL2["set0036_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0036_s.bin"]        = "CannonsCoreTails.lvl2";
    Global::levelSetToLVL2["set0036_u.bin"]        = "CannonsCoreTails.lvl2";
    Global::levelSetToLVL2["set0037_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0037_s.bin"]        = "CannonsCoreRouge.lvl2";
    Global::levelSetToLVL2["set0037_u.bin"]        = "CannonsCoreRouge.lvl2";
    Global::levelSetToLVL2["set0038_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0038_s.bin"]        = "CannonsCoreKnuckles.lvl2";
    Global::levelSetToLVL2["set0038_u.bin"]        = "CannonsCoreKnuckles.lvl2";
    Global::levelSetToLVL2["set0039_s.bin"]        = "MissionStreet2P.lvl2";
    Global::levelSetToLVL2["SET0039_U.BIN"]        = "MissionStreet2P.lvl2";
    Global::levelSetToLVL2["set0040_2p_s.bin"]     = "";
    Global::levelSetToLVL2["set0040_2p_u.bin"]     = "";
    Global::levelSetToLVL2["set0040_hd_s.bin"]     = "";
    Global::levelSetToLVL2["set0040_hd_u.bin"]     = "";
    Global::levelSetToLVL2["set0040_s.bin"]        = "FinalChase.lvl2";
    Global::levelSetToLVL2["set0040_u.bin"]        = "FinalChase.lvl2";
    Global::levelSetToLVL2["SET0041_2P_S.BIN"]     = "";
    Global::levelSetToLVL2["set0041_2p_u.bin"]     = "";
    Global::levelSetToLVL2["SET0042_S.BIN"]        = "";
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
    Global::levelSetToLVL2["set0045_s.bin"]        = "SandOcean2P.lvl2";
    Global::levelSetToLVL2["set0045_u.bin"]        = "SandOcean2P.lvl2";
    Global::levelSetToLVL2["SET0046_2P_S.BIN"]     = "";
    Global::levelSetToLVL2["SET0046_2P_U.BIN"]     = "";
    Global::levelSetToLVL2["SET0046_S.BIN"]        = "";
    Global::levelSetToLVL2["SET0046_U.BIN"]        = "";
    Global::levelSetToLVL2["SET0047_2P_S.BIN"]     = "";
    Global::levelSetToLVL2["SET0047_2P_U.BIN"]     = "";
    Global::levelSetToLVL2["SET0047_S.BIN"]        = "";
    Global::levelSetToLVL2["SET0047_U.BIN"]        = "";
    Global::levelSetToLVL2["SET0048_2P_S.BIN"]     = "";
    Global::levelSetToLVL2["SET0048_2P_U.BIN"]     = "";
    Global::levelSetToLVL2["SET0048_S.BIN"]        = "";
    Global::levelSetToLVL2["SET0048_U.BIN"]        = "";
    Global::levelSetToLVL2["SET0049_2P_S.BIN"]     = "";
    Global::levelSetToLVL2["SET0049_2P_U.BIN"]     = "";
    Global::levelSetToLVL2["SET0049_S.BIN"]        = "";
    Global::levelSetToLVL2["SET0049_U.BIN"]        = "";
    Global::levelSetToLVL2["SET0050_2P_S.BIN"]     = "";
    Global::levelSetToLVL2["SET0050_2P_U.BIN"]     = "";
    Global::levelSetToLVL2["SET0050_S.BIN"]        = "";
    Global::levelSetToLVL2["SET0050_U.BIN"]        = "";
    Global::levelSetToLVL2["SET0051_2P_S.BIN"]     = "";
    Global::levelSetToLVL2["SET0051_2P_U.BIN"]     = "";
    Global::levelSetToLVL2["SET0051_S.BIN"]        = "";
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
    //SPIDERWEB::deleteStaticModels();
    //TORCHCUP::deleteStaticModels();
    //SNAKESTATUE::deleteStaticModels();
    //SNEAKRAIL::deleteStaticModels();
    //CW_STAGE::deleteStaticModels();
}

int LevelLoader::getNumLevels()
{
    return LevelLoader::numLevels;
}

std::string LevelLoader::getObjectName(int levelID, int objectID)
{
    int key = (levelID << 8) | objectID;
    if (LevelLoader::objectIdToName.find(key) == LevelLoader::objectIdToName.end())
    {
        return "Unknown";
    }
    return LevelLoader::objectIdToName[key];
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
                MessageBox(NULL, "You only selected one file. Please select both setXXXX_s.bin and setXXXX_u.bin", "silly", MB_OK);
                return;
            }
            else
            {
                file2 = &filePaths[i+1];
                break;
            }
        }
    }

    //make them lowercase
    //std::transform(file1.begin(), file1.end(), file1.begin(), [](unsigned char c) { return std::tolower(c); });
    //std::transform(file2.begin(), file2.end(), file2.begin(), [](unsigned char c) { return std::tolower(c); });

    if (Global::levelSetToLVL2.find(file1) == Global::levelSetToLVL2.end())
    {
        MessageBox(NULL, (("This program does not recognize "+file1)+" to have a SA2 level associated with it.").c_str(), "silly", MB_OK);
        return;
    }
    if (Global::levelSetToLVL2.find(file2) == Global::levelSetToLVL2.end())
    {
        MessageBox(NULL, (("This program does not recognize "+file2)+" to have a SA2 level associated with it.").c_str(), "silly", MB_OK);
        return;
    }

    std::string lvl1 = Global::levelSetToLVL2[file1];
    std::string lvl2 = Global::levelSetToLVL2[file2];

    if (lvl1 != lvl2)
    {
        MessageBox(NULL, "These setfiles aren't associated with the same SA2 level.", "silly", MB_OK);
        return;
    }

    if (lvl1 == "" ||
        lvl2 == "")
    {
        MessageBox(NULL, "This level is not supported at this time.", "silly", MB_OK);
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

void LevelLoader::promptUserForLevelSAB()
{
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
    ofns.lpstrInitialDir = "E:\\CWorkspace\\NewSonicThing\\NewSonicThing\\res\\Levels";
    ofns.lpstrFilter = "LVL files (*.lvl)\0*.lvl\0";
    ofns.nFilterIndex = 1; //default filter to show
    ofns.lpstrTitle = "Select the .lvl file";
    ofns.Flags = OFN_NOCHANGEDIR | OFN_EXPLORER;

    bool opened = GetOpenFileName(&ofns);
    if (!opened)
    {
        return;
    }

    std::string lvlFilename = &filePaths[0];



    char filePaths2[BUFSIZE] = {0};
    OPENFILENAME ofns2 = {0};
    ofns2.lStructSize = sizeof(OPENFILENAME);
    ofns2.lpstrFile = filePaths2;
    ofns2.nMaxFile = BUFSIZE;
    ofns2.lpstrInitialDir = "E:\\CWorkspace\\NewSonicThing\\NewSonicThing\\res\\Models\\Levels\\";
    ofns2.lpstrFilter = "OBJ files (*.obj)\0*.obj\0";
    ofns2.nFilterIndex = 1; //default filter to show
    ofns2.lpstrTitle = "Select the .obj file";
    ofns2.Flags = OFN_NOCHANGEDIR | OFN_EXPLORER;

    bool opened2 = GetOpenFileName(&ofns2);
    if (!opened2)
    {
        return;
    }

    std::string objFile = &filePaths2[0];

    LevelLoader::loadLevelSAB(lvlFilename, objFile);
    Global::redrawWindow = true;
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
                MessageBox(NULL, "You only selected one file. Please select both setXXXX_s.bin and setXXXX_u.bin", "silly", MB_OK);
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
        MessageBox(NULL, "Cannot determine which file is the U file and which is the S file.", "silly", MB_OK);
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
        MessageBox(NULL, "Error: Less than 2 objects to export. Each setfile needs to have at least 1", "silly", MB_OK);
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
        MessageBox(NULL, (("Cannot open file '"+exp)+"' for writing.").c_str(), "silly", MB_OK);
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
        MessageBox(NULL, (("Cannot open file '"+exp)+"' for writing.").c_str(), "silly", MB_OK);
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
    for (int i = countU; i < (int)sa2Objects.size(); i++)
    {
        SA2Object* o = sa2Objects[i];
        memset(buf, 0, 32);
        o->fillData(buf);
        buf[0] = 0x00; //first byte of S file is 0x00
        myfileS.write(buf, 32);
    }

    myfileS.close();
}

void LevelLoader::exportLevelSAB()
{
    const int BUFSIZE = 1024;
    char filePaths[BUFSIZE] = {0};
    OPENFILENAME ofns = {0};
    ofns.lStructSize = sizeof(OPENFILENAME);
    ofns.lpstrFile = filePaths;
    ofns.nMaxFile = BUFSIZE;
    ofns.lpstrInitialDir = "E:\\CWorkspace\\NewSonicThing\\NewSonicThing\\res\\Levels";
    ofns.lpstrFilter = "LVL files (*.lvl)\0*.lvl\0";
    ofns.nFilterIndex = 1; //default filter to show
    ofns.lpstrTitle = "Select the .lvl file to export to";
    ofns.Flags = OFN_NOCHANGEDIR | OFN_EXPLORER;

    bool opened = GetOpenFileName(&ofns);
    if (!opened)
    {
        return;
    }

    std::string lvlFilename = &filePaths[0];

    std::ofstream out(lvlFilename, std::ios::out);
    if (!out.is_open())
    {
        MessageBox(NULL, (("Cannot open file '"+lvlFilename)+"' for writing.").c_str(), "silly", MB_OK);
        return;
    }

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

    //complete lvl file structuree
    for (int i = 0; i < (int)sa2Objects.size(); i++)
    {
        SA2Object* o = sa2Objects[i];

        if (o->lvlLineNum >= 0)
        {
            lvlFile[o->lvlLineNum] = o->toSabString();
        }
        else
        {
            lvlFile.push_back(o->toSabString());
            o->lvlLineNum = (int)lvlFile.size()-1;
        }
    }

    //write to the file
    for (int i = 0; i < lvlFile.size(); i++)
    {
        std::string line = lvlFile[i] + "\n";
        out.write(line.c_str(), line.size());
    }

    out.close();
}
