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
class Ghost;

#include <Windows.h>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <list>
#include "../toolbox/level.h"
#include "../toolbox/vector.h"

#define STATE_RUNNING 1
#define STATE_EXITING 2

//#define OBS_MODE
//#define DEV_MODE
//#define SAB_LVL
//#define SAB_GUIDES

#ifdef DEV_MODE
    #define INCR_NEW(NAME) Global::debugNew(NAME);
    #define INCR_DEL(NAME) Global::debugDel(NAME);
#else
    #define INCR_NEW(NAME) ;
    #define INCR_DEL(NAME) ;
#endif

#define CMD_FILE_LOAD               1
#define CMD_FILE_EXPORT             2
#define CMD_FILE_EXIT               3
#define CMD_VIEW_STAGE              4
#define CMD_VIEW_COLLISION          5
#define CMD_VIEW_KILLPLANES         6
#define CMD_VIEW_OBJECTS            7
#define CMD_VIEW_BACKGROUND         8
#define CMD_VIEW_CAMERA_TRIGGER     9
#define CMD_VIEW_LOOPSPEED_TRIGGER 10
#define CMD_VIEW_CULLING           11
#define CMD_SA2_FOLLOW             12
#define CMD_SA2_FOLLOW_NO_CAM      13
#define CMD_SA2_FOLLOW_ORBIT_CAM   14
#define CMD_SA2_TELEPORT           15
#define CMD_HELP                   16

#define CMD_BTN_1  20
#define CMD_BTN_2  21
#define CMD_BTN_3  22
#define CMD_BTN_4  23
#define CMD_BTN_5  24
#define CMD_BTN_6  25
#define CMD_BTN_7  26
#define CMD_BTN_8  27
#define CMD_BTN_9  28
#define CMD_BTN_10 29
#define CMD_BTN_11 30

// 3 slots as keys 1-3 are used for user ghosts
#define NUM_GHOST_SLOTS 3

class Global
{
public:
    static std::string version;

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

    //inputs
    static bool isHoldingX;
    static bool isHoldingY;
    static bool isHoldingZ;
    static bool isHoldingAlt;
    static bool isHoldingShift;
    static bool isHoldingClickRight;

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
    static bool displayCameraTriggers;
    static bool displayLoopspeedTriggers;
    static bool displayStage;
    static bool displayStageCollision;
    static bool displayStageKillplanes;
    static bool displayObjects;
    static bool displayStageSky;
    static bool renderWithCulling;

    static std::vector<Ghost*> userGhosts;
    static void createGhost(short slot);
    static void deleteGhost(short slot);

    //if this is true, the editor will try to attach to a running SA2 process
    // and follow the camera
    static bool gameIsFollowingSA2;
    static bool gameIsFollowingSA2NoCam;
    static bool gameIsFollowingSA2OrbitCam;
    static float orbitCamRadius;
    static Vector3f orbitCamDirection;
    static int sa2Type;
    enum SA2Type
    {
        None    = 0,
        Steam   = 1,
        Dolphin = 2,
        Sadx    = 3
    };

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
        Sand_Ocean_2P           = 45,
        Custom_Level            = 420
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
