#ifndef LEVELLOADER_H
#define LEVELLOADER_H

#include <string>
#include <list>
#include <vector>
#include <unordered_map>

class Entity;
class SA2Object;

class LevelLoader
{
private:
    static std::unordered_map<int, std::string> objectIdToName;

    static void initObjectMap();

    static float toFloat(char* input);

    static int toInt(char* input);

    static void processLine(char** data, int dataLength);

    static void processObjectSET(char data[32]);

    static void processCameraTrigger(char** data, int dataLength);

    static void processLoopspeedTrigger(char** data, int dataLength);

    static void loadLevel(std::string setDir, std::string setS, std::string setU);

    static void loadLevelSAB(std::string fileLvl, std::string fileObj);

    static void freeAllStaticModels();

    static int numLevels;

public:
    static void loadLevelData();

    static std::vector<std::string> lvlFile;

    static void loadTitle();

    static SA2Object* newSA2Object(int levelID, int objectID, char data[32], bool useDefaultValues);

    static void promptUserForLevel();

    static void promptUserForLevelSAB();

    static int getNumLevels();

    static std::string getObjectName(int levelID, int objectID);

    static void exportLevel();

    static void exportLevelSAB();
};
#endif
