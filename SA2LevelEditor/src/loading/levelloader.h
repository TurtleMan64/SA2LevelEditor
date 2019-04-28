#ifndef LEVELLOADER_H
#define LEVELLOADER_H

#include <string>
#include <list>

class Entity;

class LevelLoader
{
private:
	static float toFloat(char* input);

	static int toInt(char* input);

	static void processLine(char** data, int dataLength);

    static void processObjectSET(char data[32]);

	static void freeAllStaticModels();

	static int numLevels;

public:
	static void loadTitle();

    static void loadLevel(std::string setDir, std::string setS, std::string setU);

    static void promptUserForLevel();

	static void loadLevelData();

	static int getNumLevels();

    static void exportLevel();
};
#endif
