#ifndef MAIN_H
#define MAIN_H

class Entity;
class Camera;
class Stage;
class Cursor3D;
class StageCollision;
class StageKillplanes;
class SA2Object;
class StageSky;
class Dummy;

#include <Windows.h>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <list>
#include "../toolbox/level.h"

#define STATE_RUNNING 1
#define STATE_EXITING 2

#define DEV_MODE

#ifdef DEV_MODE
	#define INCR_NEW(NAME) Global::debugNew(NAME);
	#define INCR_DEL(NAME) Global::debugDel(NAME);
#else
	#define INCR_NEW(NAME) ;
	#define INCR_DEL(NAME) ;
#endif

class Global
{
public:
    static std::unordered_set<Entity*> gameEntities;
    static std::list<Entity*> gameEntitiesToAdd;
    static std::list<Entity*> gameEntitiesToDelete;
    static std::unordered_set<Entity*> gameEntitiesPass2;
    static std::list<Entity*> gameEntitiesPass2ToAdd;
    static std::list<Entity*> gameEntitiesPass2ToDelete;
    static std::unordered_set<Entity*> gameEntitiesPass3;
    static std::list<Entity*> gameEntitiesPass3ToAdd;
    static std::list<Entity*> gameEntitiesPass3ToDelete;
    static std::unordered_set<Entity*> gameTransparentEntities;
    static std::list<Entity*> gameTransparentEntitiesToAdd;
    static std::list<Entity*> gameTransparentEntitiesToDelete;

    static void updateCamFromSA2();

    static void teleportSA2PlayerToCursor3D();

public:
    static int main();

    static int initWin32GUI(HINSTANCE hInstance);

    //when there is no object selected, call this to
    // reset the values to blank and make them not editable.
    static void resetObjectWindow();

    static HWND mainWindow;
    static HMENU mainMenu;
    static HMENU mainMenuFile;
    static HMENU mainMenuView;
    static HMENU mainMenuSA2;
    static std::vector<HWND> windowLabels;
    static std::vector<HWND> windowValues;
    static std::vector<HWND> windowButtons;
    static std::vector<HWND> windowDescriptions;

    static bool redrawWindow;

    //The currently selected object
    static SA2Object* selectedSA2Object;
    static bool isMovingX;
    static bool isMovingY;
    static bool isMovingZ;

	static Camera* gameCamera;
	static Stage* gameStage;
    static StageCollision* gameStageCollision;
    static StageKillplanes* gameStageKillplanes;
	static StageSky* gameStageSky;
    static Cursor3D* gameCursor3D;
    static Dummy* gamePlayer;
	static int countNew;
	static int countDelete;
	static int gameState;

    //if this is true, the editor will try to attach to a running SA2 process
    // and follow the camera
    static bool gameIsFollowingSA2;

    enum Levels
    {
        Basic_Test              = 0 ,
        Knuckles_Test           = 1 ,
        Sonic_Test              = 2 ,
        Green_Forest            = 3 ,
        White_Jungle            = 4 ,
        Pumpkin_Hill            = 5 ,
        Sky_Rail                = 6 ,
        Aquatic_Mine            = 7 ,
        Security_Hall           = 8 ,
        Prison_Lane             = 9 ,
        Metal_Harbor            = 10,
        Iron_Gate               = 11,
        Weapons_Bed             = 12,
        City_Escape             = 13,
        Radical_Highway         = 14,
        Weapons_Bed_2P          = 15,
        Wild_Canyon             = 16,
        Mission_Street          = 17,
        Dry_Lagoon              = 18,
        Sonic_Vs_Shadow_1       = 19,
        Tails_Vs_Eggman_1       = 20,
        Sand_Ocean              = 21,
        Crazy_Gadget            = 22,
        Hidden_Base             = 23,
        Eternal_Engine          = 24,
        Death_Chamber           = 25,
        Egg_Quarters            = 26,
        Lost_Colony             = 27,
        Pyramid_Cave            = 28,
        Tails_Vs_Eggman_2       = 29,
        Final_Rush              = 30,
        Green_Hill              = 31,
        Meteor_Herd             = 32,
        Knuckles_Vs_Rouge       = 33,
        Cannons_Core_Sonic      = 34,
        Cannons_Core_Eggman     = 35,
        Cannons_Core_Tails      = 36,
        Cannons_Core_Rouge      = 37,
        Cannons_Core_Knuckles   = 38,
        Mission_Street_2P       = 39,
        Final_Chase             = 40,
        Wild_Canyon_2P          = 41,
        Sonic_Vs_Shadow_2       = 42,
        Cosmic_Wall             = 43,
        Mad_Space               = 44,
        Sand_Ocean_2P           = 45
    };

	static int levelID;
    static bool shouldLoadNewLevel;
    static bool shouldExportLevel;

	static int gameMissionNumber;
    static std::string dirSA2Root;
    static std::string dirProgRoot;
    static std::string dirSet;

    static std::unordered_map<std::string, std::string> levelSetToLVL2;

    static void debugNew(const char* name);
    static void debugDel(const char* name);

    static void addEntity(Entity* entityToAdd);
    static void deleteEntity(Entity* entityToDelete);
    static void deleteAllEntites();

    static void addEntityPass2(Entity* entityToAdd);
    static void deleteEntityPass2(Entity* entityToDelete);
    static void deleteAllEntitesPass2();

    static void addEntityPass3(Entity* entityToAdd);
    static void deleteEntityPass3(Entity* entityToDelete);
    static void deleteAllEntitesPass3();

    static void addTransparentEntity(Entity* entityToAdd);
    static void deleteTransparentEntity(Entity* entityToDelete);
    static void deleteAllTransparentEntites();
};
#endif