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
//#include "../particles/particlemaster.h"
//#include "../particles/particleresources.h"
#include "../toolbox/split.h"
#include "../toolbox/level.h"
//#include "../guis/guitexture.h"
//#include "../particles/particle.h"
//#include "../entities/skysphere.h"
#include "../toolbox/getline.h"
#include "../rendering/masterrenderer.h"
#include "../guis/guimanager.h"
#include "../guis/guirenderer.h"
#include "../guis/guitextureresources.h"
#include "../loading/loader.h"
#include "../entities/cursor3d.h"
#include "../entities/stagecollision.h"
#include "../entities/stagekillplanes.h"
#include "../entities/stagesky.h"
#include "../entities/GlobalObjects/ring.h"
#include "../entities/GlobalObjects/sprb.h"
#include "../entities/GlobalObjects/spra.h"
#include "../entities/unknown.h"
#include "../toolbox/maths.h"
#include "../loading/objloader.h"
#include "../entities/dummy.h"

#ifdef _WIN32
#include <Windows.h>
#include <commdlg.h>
#include <tchar.h>
#include <tlhelp32.h>
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
bool Global::redrawWindow = true;
SA2Object* Global::selectedSA2Object = nullptr;
bool Global::isMovingX = false;
bool Global::isMovingY = false;
bool Global::isMovingZ = false;

Camera*          Global::gameCamera          = nullptr;
Stage*           Global::gameStage           = nullptr;
StageCollision*  Global::gameStageCollision  = nullptr;
StageKillplanes* Global::gameStageKillplanes = nullptr;
StageSky*        Global::gameStageSky        = nullptr;
Cursor3D*        Global::gameCursor3D        = nullptr;
Dummy*           Global::gamePlayer          = nullptr;

std::list<TexturedModel*> playerModels;

std::string Global::dirSA2Root = "C:/Program Files (x86)/Steam/steamapps/common/Sonic Adventure 2";
std::string Global::dirProgRoot = "";
std::string Global::dirSet = "";

int Global::countNew = 0;
int Global::countDelete = 0;
int Global::gameState = 0;
int Global::levelID = 0;
bool Global::shouldLoadNewLevel = false;
bool Global::shouldExportLevel  = false;
bool Global::gameIsFollowingSA2 = false;

int Global::gameMissionNumber = 0;
std::unordered_map<std::string, std::string> Global::levelSetToLVL2;

HWND  Global::mainWindow   = nullptr;
HMENU Global::mainMenu     = nullptr;
HMENU Global::mainMenuFile = nullptr;
HMENU Global::mainMenuView = nullptr;
HMENU Global::mainMenuSA2  = nullptr;
std::vector<HWND> Global::windowLabels;
std::vector<HWND> Global::windowValues;
std::vector<HWND> Global::windowButtons;
std::vector<HWND> Global::windowDescriptions;

void addMenus(HWND window);
void addControls(HWND window);

//entry point of the program
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
    //remake the console, since it somehow becomes missing once we enter the program here
#ifdef DEV_MODE
    AllocConsole();
    freopen("CONIN$", "r",stdin);
    freopen("CONOUT$", "w",stdout);
    freopen("CONOUT$", "w",stderr);
#endif

    if (Global::initWin32GUI(hInstance) < 0)
    {
        MessageBox(NULL, "RegisterClassW failed. Not sure why. Program can't start without that though, so I have to quit.", "Fatal Error", MB_OK);
        return -1;
    }

    return Global::main();
}

#define CMD_FILE_LOAD       1
#define CMD_FILE_EXPORT     2
#define CMD_FILE_EXIT       3
#define CMD_VIEW_STAGE      4
#define CMD_VIEW_COLLISION  5
#define CMD_VIEW_KILLPLANES 6
#define CMD_VIEW_BACKGROUND 7
#define CMD_SA2_FOLLOW      8
#define CMD_SA2_TELEPORT    9
#define CMD_HELP           10

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

void addMenus(HWND window)
{
    Global::mainMenu     = CreateMenu();
    Global::mainMenuFile = CreateMenu();
    Global::mainMenuView = CreateMenu();
    Global::mainMenuSA2  = CreateMenu();

    AppendMenu(Global::mainMenuFile, MF_STRING, CMD_FILE_LOAD, "Load Objects from SET Files");
    AppendMenu(Global::mainMenuFile, MF_STRING, CMD_FILE_EXPORT, "Export Objects to SET Files");
    AppendMenu(Global::mainMenuFile, MF_SEPARATOR, 0, "");
    AppendMenu(Global::mainMenuFile, MF_STRING, CMD_FILE_EXIT, "Exit");

    AppendMenu(Global::mainMenuView, MF_STRING, CMD_VIEW_STAGE     , "View Stage");
    AppendMenu(Global::mainMenuView, MF_STRING, CMD_VIEW_COLLISION , "View Collision");
    AppendMenu(Global::mainMenuView, MF_STRING, CMD_VIEW_KILLPLANES, "View Killplanes");
    AppendMenu(Global::mainMenuView, MF_STRING, CMD_VIEW_BACKGROUND, "View Background");
    CheckMenuItem(Global::mainMenuView, CMD_VIEW_STAGE     , MF_CHECKED);
    CheckMenuItem(Global::mainMenuView, CMD_VIEW_COLLISION , MF_CHECKED);
    CheckMenuItem(Global::mainMenuView, CMD_VIEW_KILLPLANES, MF_CHECKED);
    CheckMenuItem(Global::mainMenuView, CMD_VIEW_BACKGROUND, MF_CHECKED);

    AppendMenu(Global::mainMenuSA2, MF_STRING, CMD_SA2_FOLLOW,   "Follow SA2 in Real Time");
    AppendMenu(Global::mainMenuSA2, MF_STRING, CMD_SA2_TELEPORT, "Teleport Playable Character to 3D Cursor");
    CheckMenuItem(Global::mainMenuSA2, CMD_SA2_FOLLOW, MF_UNCHECKED);

    AppendMenu(Global::mainMenu, MF_POPUP, (UINT_PTR)Global::mainMenuFile, "File");
    AppendMenu(Global::mainMenu, MF_POPUP, (UINT_PTR)Global::mainMenuView, "View");
    AppendMenu(Global::mainMenu, MF_POPUP, (UINT_PTR)Global::mainMenuSA2, "SA2");
    AppendMenu(Global::mainMenu, MF_STRING, CMD_HELP, "Help");

    SetMenu(window, Global::mainMenu);
}

void addControls(HWND window)
{
    //CreateWindowW(L"static", L"Enter text here:", WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER, 200, 100, 100, 50, window, NULL, NULL, NULL);
    //CreateWindowW(L"Edit", L"...", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL, 200, 152, 100, 50, window, NULL, NULL, NULL);
    int entryHOffset = 60;
    int pad = 2;

    int entryHeight = 20;
    int entryWidth = 80;
    Global::windowLabels.push_back(CreateWindowW(L"STATIC", L"ID"        , WS_VISIBLE | WS_CHILD | WS_BORDER, 0+pad,  0*entryHOffset+( 1*pad), entryWidth, entryHeight, window, NULL, NULL, NULL));
    Global::windowLabels.push_back(CreateWindowW(L"STATIC", L"Name"      , WS_VISIBLE | WS_CHILD | WS_BORDER, 0+pad,  1*entryHOffset+( 2*pad), entryWidth, entryHeight, window, NULL, NULL, NULL));
    Global::windowLabels.push_back(CreateWindowW(L"STATIC", L"Position X", WS_VISIBLE | WS_CHILD | WS_BORDER, 0+pad,  2*entryHOffset+( 3*pad), entryWidth, entryHeight, window, NULL, NULL, NULL));
    Global::windowLabels.push_back(CreateWindowW(L"STATIC", L"Position Y", WS_VISIBLE | WS_CHILD | WS_BORDER, 0+pad,  3*entryHOffset+( 4*pad), entryWidth, entryHeight, window, NULL, NULL, NULL));
    Global::windowLabels.push_back(CreateWindowW(L"STATIC", L"Position Z", WS_VISIBLE | WS_CHILD | WS_BORDER, 0+pad,  4*entryHOffset+( 5*pad), entryWidth, entryHeight, window, NULL, NULL, NULL));
    Global::windowLabels.push_back(CreateWindowW(L"STATIC", L"Rotation X", WS_VISIBLE | WS_CHILD | WS_BORDER, 0+pad,  5*entryHOffset+( 6*pad), entryWidth, entryHeight, window, NULL, NULL, NULL));
    Global::windowLabels.push_back(CreateWindowW(L"STATIC", L"Rotation Y", WS_VISIBLE | WS_CHILD | WS_BORDER, 0+pad,  6*entryHOffset+( 7*pad), entryWidth, entryHeight, window, NULL, NULL, NULL));
    Global::windowLabels.push_back(CreateWindowW(L"STATIC", L"Rotation Z", WS_VISIBLE | WS_CHILD | WS_BORDER, 0+pad,  7*entryHOffset+( 8*pad), entryWidth, entryHeight, window, NULL, NULL, NULL));
    Global::windowLabels.push_back(CreateWindowW(L"STATIC", L"Variable 1", WS_VISIBLE | WS_CHILD | WS_BORDER, 0+pad,  8*entryHOffset+( 9*pad), entryWidth, entryHeight, window, NULL, NULL, NULL));
    Global::windowLabels.push_back(CreateWindowW(L"STATIC", L"Variable 2", WS_VISIBLE | WS_CHILD | WS_BORDER, 0+pad,  9*entryHOffset+(10*pad), entryWidth, entryHeight, window, NULL, NULL, NULL));
    Global::windowLabels.push_back(CreateWindowW(L"STATIC", L"Variable 3", WS_VISIBLE | WS_CHILD | WS_BORDER, 0+pad, 10*entryHOffset+(11*pad), entryWidth, entryHeight, window, NULL, NULL, NULL));

    int entry2Height = 20;
    int entry2Width = 156;
    Global::windowValues.push_back(CreateWindowW(L"EDIT",   L"", WS_VISIBLE | WS_CHILD | WS_BORDER, entryWidth+2*pad,  0*entryHOffset+( 1*pad), entry2Width, entry2Height, window, NULL, NULL, NULL));
    Global::windowValues.push_back(CreateWindowW(L"STATIC", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, entryWidth+2*pad,  1*entryHOffset+( 2*pad), entry2Width, entry2Height, window, NULL, NULL, NULL));
    Global::windowValues.push_back(CreateWindowW(L"EDIT",   L"", WS_VISIBLE | WS_CHILD | WS_BORDER, entryWidth+2*pad,  2*entryHOffset+( 3*pad), entry2Width, entry2Height, window, NULL, NULL, NULL));
    Global::windowValues.push_back(CreateWindowW(L"EDIT",   L"", WS_VISIBLE | WS_CHILD | WS_BORDER, entryWidth+2*pad,  3*entryHOffset+( 4*pad), entry2Width, entry2Height, window, NULL, NULL, NULL));
    Global::windowValues.push_back(CreateWindowW(L"EDIT",   L"", WS_VISIBLE | WS_CHILD | WS_BORDER, entryWidth+2*pad,  4*entryHOffset+( 5*pad), entry2Width, entry2Height, window, NULL, NULL, NULL));
    Global::windowValues.push_back(CreateWindowW(L"EDIT",   L"", WS_VISIBLE | WS_CHILD | WS_BORDER, entryWidth+2*pad,  5*entryHOffset+( 6*pad), entry2Width, entry2Height, window, NULL, NULL, NULL));
    Global::windowValues.push_back(CreateWindowW(L"EDIT",   L"", WS_VISIBLE | WS_CHILD | WS_BORDER, entryWidth+2*pad,  6*entryHOffset+( 7*pad), entry2Width, entry2Height, window, NULL, NULL, NULL));
    Global::windowValues.push_back(CreateWindowW(L"EDIT",   L"", WS_VISIBLE | WS_CHILD | WS_BORDER, entryWidth+2*pad,  7*entryHOffset+( 8*pad), entry2Width, entry2Height, window, NULL, NULL, NULL));
    Global::windowValues.push_back(CreateWindowW(L"EDIT",   L"", WS_VISIBLE | WS_CHILD | WS_BORDER, entryWidth+2*pad,  8*entryHOffset+( 9*pad), entry2Width, entry2Height, window, NULL, NULL, NULL));
    Global::windowValues.push_back(CreateWindowW(L"EDIT",   L"", WS_VISIBLE | WS_CHILD | WS_BORDER, entryWidth+2*pad,  9*entryHOffset+(10*pad), entry2Width, entry2Height, window, NULL, NULL, NULL));
    Global::windowValues.push_back(CreateWindowW(L"EDIT",   L"", WS_VISIBLE | WS_CHILD | WS_BORDER, entryWidth+2*pad, 10*entryHOffset+(11*pad), entry2Width, entry2Height, window, NULL, NULL, NULL));

    Global::windowButtons.push_back(CreateWindowW(L"BUTTON", L"Update", WS_VISIBLE | WS_CHILD, entryWidth+2*pad,  0*entryHOffset+( 2*pad)+entry2Height, entry2Width, entry2Height, window, (HMENU)CMD_BTN_1,  NULL, NULL));
    Global::windowButtons.push_back(CreateWindowW(L"BUTTON", L"Update",              WS_CHILD, entryWidth+2*pad,  1*entryHOffset+( 3*pad)+entry2Height, entry2Width, entry2Height, window, (HMENU)CMD_BTN_2,  NULL, NULL));
    Global::windowButtons.push_back(CreateWindowW(L"BUTTON", L"Update", WS_VISIBLE | WS_CHILD, entryWidth+2*pad,  2*entryHOffset+( 4*pad)+entry2Height, entry2Width, entry2Height, window, (HMENU)CMD_BTN_3,  NULL, NULL));
    Global::windowButtons.push_back(CreateWindowW(L"BUTTON", L"Update", WS_VISIBLE | WS_CHILD, entryWidth+2*pad,  3*entryHOffset+( 5*pad)+entry2Height, entry2Width, entry2Height, window, (HMENU)CMD_BTN_4,  NULL, NULL));
    Global::windowButtons.push_back(CreateWindowW(L"BUTTON", L"Update", WS_VISIBLE | WS_CHILD, entryWidth+2*pad,  4*entryHOffset+( 6*pad)+entry2Height, entry2Width, entry2Height, window, (HMENU)CMD_BTN_5,  NULL, NULL));
    Global::windowButtons.push_back(CreateWindowW(L"BUTTON", L"Update", WS_VISIBLE | WS_CHILD, entryWidth+2*pad,  5*entryHOffset+( 7*pad)+entry2Height, entry2Width, entry2Height, window, (HMENU)CMD_BTN_6,  NULL, NULL));
    Global::windowButtons.push_back(CreateWindowW(L"BUTTON", L"Update", WS_VISIBLE | WS_CHILD, entryWidth+2*pad,  6*entryHOffset+( 8*pad)+entry2Height, entry2Width, entry2Height, window, (HMENU)CMD_BTN_7,  NULL, NULL));
    Global::windowButtons.push_back(CreateWindowW(L"BUTTON", L"Update", WS_VISIBLE | WS_CHILD, entryWidth+2*pad,  7*entryHOffset+( 9*pad)+entry2Height, entry2Width, entry2Height, window, (HMENU)CMD_BTN_8,  NULL, NULL));
    Global::windowButtons.push_back(CreateWindowW(L"BUTTON", L"Update", WS_VISIBLE | WS_CHILD, entryWidth+2*pad,  8*entryHOffset+(10*pad)+entry2Height, entry2Width, entry2Height, window, (HMENU)CMD_BTN_9,  NULL, NULL));
    Global::windowButtons.push_back(CreateWindowW(L"BUTTON", L"Update", WS_VISIBLE | WS_CHILD, entryWidth+2*pad,  9*entryHOffset+(11*pad)+entry2Height, entry2Width, entry2Height, window, (HMENU)CMD_BTN_10, NULL, NULL));
    Global::windowButtons.push_back(CreateWindowW(L"BUTTON", L"Update", WS_VISIBLE | WS_CHILD, entryWidth+2*pad, 10*entryHOffset+(12*pad)+entry2Height, entry2Width, entry2Height, window, (HMENU)CMD_BTN_11, NULL, NULL));

    int entry3Height = 60;
    int entry3Width = 320;
    Global::windowDescriptions.push_back(CreateWindowW(L"STATIC", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, entryWidth+entry2Width+3*pad,  0*entryHOffset+( 1*pad), entry3Width, entry3Height, window, NULL, NULL, NULL));
    Global::windowDescriptions.push_back(CreateWindowW(L"STATIC", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, entryWidth+entry2Width+3*pad,  1*entryHOffset+( 2*pad), entry3Width, entry3Height, window, NULL, NULL, NULL));
    Global::windowDescriptions.push_back(CreateWindowW(L"STATIC", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, entryWidth+entry2Width+3*pad,  2*entryHOffset+( 3*pad), entry3Width, entry3Height, window, NULL, NULL, NULL));
    Global::windowDescriptions.push_back(CreateWindowW(L"STATIC", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, entryWidth+entry2Width+3*pad,  3*entryHOffset+( 4*pad), entry3Width, entry3Height, window, NULL, NULL, NULL));
    Global::windowDescriptions.push_back(CreateWindowW(L"STATIC", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, entryWidth+entry2Width+3*pad,  4*entryHOffset+( 5*pad), entry3Width, entry3Height, window, NULL, NULL, NULL));
    Global::windowDescriptions.push_back(CreateWindowW(L"STATIC", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, entryWidth+entry2Width+3*pad,  5*entryHOffset+( 6*pad), entry3Width, entry3Height, window, NULL, NULL, NULL));
    Global::windowDescriptions.push_back(CreateWindowW(L"STATIC", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, entryWidth+entry2Width+3*pad,  6*entryHOffset+( 7*pad), entry3Width, entry3Height, window, NULL, NULL, NULL));
    Global::windowDescriptions.push_back(CreateWindowW(L"STATIC", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, entryWidth+entry2Width+3*pad,  7*entryHOffset+( 8*pad), entry3Width, entry3Height, window, NULL, NULL, NULL));
    Global::windowDescriptions.push_back(CreateWindowW(L"STATIC", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, entryWidth+entry2Width+3*pad,  8*entryHOffset+( 9*pad), entry3Width, entry3Height, window, NULL, NULL, NULL));
    Global::windowDescriptions.push_back(CreateWindowW(L"STATIC", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, entryWidth+entry2Width+3*pad,  9*entryHOffset+(10*pad), entry3Width, entry3Height, window, NULL, NULL, NULL));
    Global::windowDescriptions.push_back(CreateWindowW(L"STATIC", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, entryWidth+entry2Width+3*pad, 10*entryHOffset+(11*pad), entry3Width, entry3Height, window, NULL, NULL, NULL));
}

LRESULT CALLBACK win32WindowCallback(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_COMMAND:
        switch (wp)
        {
        case CMD_FILE_LOAD:   Global::shouldLoadNewLevel = true; break;
        case CMD_FILE_EXPORT: Global::shouldExportLevel  = true; break;
        case CMD_FILE_EXIT:   Global::gameState = STATE_EXITING; break;

        case CMD_VIEW_STAGE:
        {
            Global::gameStage->visible = !Global::gameStage->visible;
            if (Global::gameStage->visible) { CheckMenuItem(Global::mainMenuView, CMD_VIEW_STAGE, MF_CHECKED  ); }
            else                            { CheckMenuItem(Global::mainMenuView, CMD_VIEW_STAGE, MF_UNCHECKED); }
            Global::redrawWindow = true;
            break;
        }
        case CMD_VIEW_COLLISION:
        {
            Global::gameStageCollision->visible = !Global::gameStageCollision->visible;
            if (Global::gameStageCollision->visible) { CheckMenuItem(Global::mainMenuView, CMD_VIEW_COLLISION, MF_CHECKED  ); }
            else                                     { CheckMenuItem(Global::mainMenuView, CMD_VIEW_COLLISION, MF_UNCHECKED); }
            Global::redrawWindow = true;
            break;
        }
        case CMD_VIEW_KILLPLANES:
        {
            Global::gameStageKillplanes->visible = !Global::gameStageKillplanes->visible;
            if (Global::gameStageKillplanes->visible) { CheckMenuItem(Global::mainMenuView, CMD_VIEW_KILLPLANES, MF_CHECKED  ); }
            else                                      { CheckMenuItem(Global::mainMenuView, CMD_VIEW_KILLPLANES, MF_UNCHECKED); }
            Global::redrawWindow = true;
            break;
        }
        case CMD_VIEW_BACKGROUND:
        {
            Global::gameStageSky->visible = !Global::gameStageSky->visible;
            if (Global::gameStageSky->visible) { CheckMenuItem(Global::mainMenuView, CMD_VIEW_BACKGROUND, MF_CHECKED  ); }
            else                               { CheckMenuItem(Global::mainMenuView, CMD_VIEW_BACKGROUND, MF_UNCHECKED); }
            Global::redrawWindow = true;
            break;
        }

        case CMD_SA2_FOLLOW:
        {
            Global::gameIsFollowingSA2 = !Global::gameIsFollowingSA2;
            if (Global::gameIsFollowingSA2) { CheckMenuItem(Global::mainMenuSA2, CMD_SA2_FOLLOW, MF_CHECKED  ); }
            else                            { CheckMenuItem(Global::mainMenuSA2, CMD_SA2_FOLLOW, MF_UNCHECKED); }
            break;
        }
        case CMD_SA2_TELEPORT: Global::teleportSA2PlayerToCursor3D(); break;
        
        case CMD_HELP: MessageBox(NULL, "Load the U and S setfile in and the rest of the stage models will load automatically. Use C, V, and B keys to toggle visibility of the killplanes/collision/stage.", "Help", MB_OK); break;
        
        case CMD_BTN_1:  if (Global::selectedSA2Object != nullptr) { Global::selectedSA2Object->updateValue(CMD_BTN_1 -CMD_BTN_1); } break;
        case CMD_BTN_2:  if (Global::selectedSA2Object != nullptr) { Global::selectedSA2Object->updateValue(CMD_BTN_2 -CMD_BTN_1); } break;
        case CMD_BTN_3:  if (Global::selectedSA2Object != nullptr) { Global::selectedSA2Object->updateValue(CMD_BTN_3 -CMD_BTN_1); } break;
        case CMD_BTN_4:  if (Global::selectedSA2Object != nullptr) { Global::selectedSA2Object->updateValue(CMD_BTN_4 -CMD_BTN_1); } break;
        case CMD_BTN_5:  if (Global::selectedSA2Object != nullptr) { Global::selectedSA2Object->updateValue(CMD_BTN_5 -CMD_BTN_1); } break;
        case CMD_BTN_6:  if (Global::selectedSA2Object != nullptr) { Global::selectedSA2Object->updateValue(CMD_BTN_6 -CMD_BTN_1); } break;
        case CMD_BTN_7:  if (Global::selectedSA2Object != nullptr) { Global::selectedSA2Object->updateValue(CMD_BTN_7 -CMD_BTN_1); } break;
        case CMD_BTN_8:  if (Global::selectedSA2Object != nullptr) { Global::selectedSA2Object->updateValue(CMD_BTN_8 -CMD_BTN_1); } break;
        case CMD_BTN_9:  if (Global::selectedSA2Object != nullptr) { Global::selectedSA2Object->updateValue(CMD_BTN_9 -CMD_BTN_1); } break;
        case CMD_BTN_10: if (Global::selectedSA2Object != nullptr) { Global::selectedSA2Object->updateValue(CMD_BTN_10-CMD_BTN_1); } break;
        case CMD_BTN_11: if (Global::selectedSA2Object != nullptr) { Global::selectedSA2Object->updateValue(CMD_BTN_11-CMD_BTN_1); } break;

        default: break;
        }
        break;

    default:
        return DefWindowProcW(hWnd, msg, wp, lp);
    }

    return 0;
}

int Global::initWin32GUI(HINSTANCE hInstance)
{
    WNDCLASSW wc = {0};

    wc.hbrBackground = CreateSolidBrush(RGB(200, 200, 200));//(HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
    wc.hInstance     = hInstance;
    wc.lpszClassName = L"SA2 Level Editor";
    wc.lpfnWndProc   = win32WindowCallback;

    if (!RegisterClassW(&wc))
    {
        return -1;
    }

    Global::mainWindow = CreateWindowW(L"SA2 Level Editor", L"SA2 Level Editor", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, 580, 743, NULL, NULL, NULL, NULL);

    addMenus(Global::mainWindow);
    addControls(Global::mainWindow);

    Global::resetObjectWindow();

    //TODO: have some sort of option that allows the user to relocate existing objects
    // to the location of the 3d cursor. Then maybe a right click could select objects and not move the 3d cursor.

    //MSG msg = {0};

    //while (GetMessage(&msg, NULL, NULL, NULL))
    //{
    //    TranslateMessage(&msg);
    //    DispatchMessage(&msg);
    //}

    return 0;
}

void Global::resetObjectWindow()
{
    SetWindowTextA(Global::windowLabels[ 0], "ID"        );
    SetWindowTextA(Global::windowLabels[ 1], "Name"      );
    SetWindowTextA(Global::windowLabels[ 2], "Position X");
    SetWindowTextA(Global::windowLabels[ 3], "Position Y");
    SetWindowTextA(Global::windowLabels[ 4], "Position Z");
    SetWindowTextA(Global::windowLabels[ 5], "Rotation X");
    SetWindowTextA(Global::windowLabels[ 6], "Rotation Y");
    SetWindowTextA(Global::windowLabels[ 7], "Rotation Z");
    SetWindowTextA(Global::windowLabels[ 8], "Variable 1");
    SetWindowTextA(Global::windowLabels[ 9], "Variable 2");
    SetWindowTextA(Global::windowLabels[10], "Variable 3");

    SetWindowTextA(Global::windowValues[ 0], "");
    SetWindowTextA(Global::windowValues[ 1], "");
    SetWindowTextA(Global::windowValues[ 2], "");
    SetWindowTextA(Global::windowValues[ 3], "");
    SetWindowTextA(Global::windowValues[ 4], "");
    SetWindowTextA(Global::windowValues[ 5], "");
    SetWindowTextA(Global::windowValues[ 6], "");
    SetWindowTextA(Global::windowValues[ 7], "");
    SetWindowTextA(Global::windowValues[ 8], "");
    SetWindowTextA(Global::windowValues[ 9], "");
    SetWindowTextA(Global::windowValues[10], "");

    SendMessageA(Global::windowValues[ 0], EM_SETREADONLY, 1, 0);
    SendMessageA(Global::windowValues[ 1], EM_SETREADONLY, 1, 0);
    SendMessageA(Global::windowValues[ 2], EM_SETREADONLY, 1, 0);
    SendMessageA(Global::windowValues[ 3], EM_SETREADONLY, 1, 0);
    SendMessageA(Global::windowValues[ 4], EM_SETREADONLY, 1, 0);
    SendMessageA(Global::windowValues[ 5], EM_SETREADONLY, 1, 0);
    SendMessageA(Global::windowValues[ 6], EM_SETREADONLY, 1, 0);
    SendMessageA(Global::windowValues[ 7], EM_SETREADONLY, 1, 0);
    SendMessageA(Global::windowValues[ 8], EM_SETREADONLY, 1, 0);
    SendMessageA(Global::windowValues[ 9], EM_SETREADONLY, 1, 0);
    SendMessageA(Global::windowValues[10], EM_SETREADONLY, 1, 0);

    SetWindowTextA(Global::windowDescriptions[ 0], "");
    SetWindowTextA(Global::windowDescriptions[ 1], "");
    SetWindowTextA(Global::windowDescriptions[ 2], "");
    SetWindowTextA(Global::windowDescriptions[ 3], "");
    SetWindowTextA(Global::windowDescriptions[ 4], "");
    SetWindowTextA(Global::windowDescriptions[ 5], "");
    SetWindowTextA(Global::windowDescriptions[ 6], "");
    SetWindowTextA(Global::windowDescriptions[ 7], "");
    SetWindowTextA(Global::windowDescriptions[ 8], "");
    SetWindowTextA(Global::windowDescriptions[ 9], "");
    SetWindowTextA(Global::windowDescriptions[10], "");
}

//int main2(int argc, char** argv)
//{
    //used to get the root directory. but now since the program
    // doesnt start here its kind of useless.
    //if (argc >= 1)
    //{
    //    std::string pathog = argv[0];
    //    const char* path = pathog.c_str();
    //    int lastFolderIdx = 0;
    //    for (int i = (int)pathog.length()-1; i >= 0; i--)
    //    {
    //        if (path[i] == '\\')
    //        {
    //            lastFolderIdx = i;
    //            break;
    //        }
    //    }
    //
    //    std::string realPath = "";
    //    for (int i = 0; i < lastFolderIdx; i++)
    //    {
    //        realPath = realPath + path[i];
    //    }
    //    Global::dirProgRoot = realPath;
    //}

    //return Global::main();
//}

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

	GuiManager::init();
    GuiTextureResources::loadGuiTextures();

    //this cursor never gets deleted
    Cursor3D cursor;
    Global::gameCursor3D = &cursor;

	CollisionChecker::initChecker();

	//This stage never gets deleted.
	Stage stage;
	Global::gameStage = &stage;

    //This stage collision never gets deleted.
    StageCollision stageCollision;
    Global::gameStageCollision = &stageCollision;

    //This stage killplanes never gets deleted.
    StageKillplanes stageKillplanes;
    Global::gameStageKillplanes = &stageKillplanes;

	//This sky never gets deleted.
	StageSky stageSky;
	Global::gameStageSky = &stageSky;

    //load all global object models
    RING::loadStaticModels();
    Unknown::loadStaticModels();
    SPRB::loadStaticModels();
    SPRA::loadStaticModels();

    loadModel(&playerModels, "res/Models/GlobalObjects/Sonic/", "Sonic");

    //This dummy never gets deleted
    Dummy player(&playerModels);
    Global::gamePlayer = &player;


	glfwSetTime(0);

	int frameCount = 0;
	double previousTime = 0;

	Global::gameState = STATE_RUNNING;

	while (Global::gameState != STATE_EXITING && DisplayManager::displayWantsToClose() == 0)
	{
		frameCount++;
		timeNew = glfwGetTime();
		dt = (float)(timeNew - timeOld);
		dt = std::fmaxf(0.001f, dt);

		timeOld = timeNew;

        if (!Global::gameIsFollowingSA2)
        {
		    Input::waitForInputs();
        }
        else
        {
            glfwPollEvents();
        }

        if (Global::shouldLoadNewLevel)
        {
            Global::shouldLoadNewLevel = false;
            LevelLoader::promptUserForLevel();
        }

        if (Global::shouldExportLevel)
        {
            Global::shouldExportLevel = false;
            LevelLoader::exportLevel();
        }

        if (!Global::redrawWindow && !Global::gameIsFollowingSA2)
        {
            continue;
        }
        Global::redrawWindow = false;

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

        if (Global::gameIsFollowingSA2)
        {
            Global::updateCamFromSA2();
        }

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

				Global::gameCamera->refresh();
                Global::gameCursor3D->step();
                Global::gameStage->step();
                Global::gameStageCollision->step();
                Global::gameStageKillplanes->step();
                Global::gameStageSky->step();

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
		
		MasterRenderer::processEntity(Global::gameStage);
        MasterRenderer::processEntity(Global::gameStageCollision);
        MasterRenderer::processEntity(Global::gameStageKillplanes);
		MasterRenderer::processEntity(Global::gameStageSky);
        MasterRenderer::processEntity(Global::gamePlayer);

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

		GuiManager::renderAll();
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
	Loader::cleanUp();
	GuiRenderer::cleanUp();
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

#ifdef DEV_MODE
void Global::debugNew(const char* name)
{
    Global::countNew++;

    if (heapObjects.find(name) == heapObjects.end())
    {
        heapObjects[name] = 1;
    }
    else
    {
        int num = heapObjects[name];
        heapObjects[name] = num+1;
    }
}

void Global::debugDel(const char* name)
{
    Global::countDelete++;

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
}
#else
void Global::debugNew(const char* )
{
    Global::countNew++;
}

void Global::debugDel(const char* )
{
    Global::countDelete++;
}
#endif


const float ATTACH_DELAY = 2.0f;
DWORD sa2PID = NULL;
HANDLE sa2Handle = NULL;
float timeUntilNextProcessAttach = ATTACH_DELAY;

DWORD getPIDByName(const char* processName)
{
    PROCESSENTRY32 pe32 = {0};
	HANDLE hSnapshot = NULL;

	pe32.dwSize = sizeof(PROCESSENTRY32);
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (Process32First(hSnapshot, &pe32))
	{
		do
        {
			if (strcmp(pe32.szExeFile, processName) == 0)
            {
				break;
            }
		}
        while (Process32Next(hSnapshot, &pe32));
	}

	if (hSnapshot != INVALID_HANDLE_VALUE)
    {
	    CloseHandle(hSnapshot);
        return pe32.th32ProcessID;
    }

	return NULL;
}

void Global::updateCamFromSA2()
{
    if (sa2Handle == NULL)
    {
        if (timeUntilNextProcessAttach <= 0.0f)
        {
            sa2PID = getPIDByName("sonic2app.exe");

            if (sa2PID == NULL)
            {
                timeUntilNextProcessAttach = ATTACH_DELAY;
            }
            else
            {
                sa2Handle = OpenProcess(PROCESS_VM_READ | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, false, sa2PID);
                if (sa2Handle == NULL)
                {
                    sa2PID = NULL;
                    timeUntilNextProcessAttach = ATTACH_DELAY;
                }
            }
        }
        else
        {
            extern float dt;
            timeUntilNextProcessAttach -= dt;
        }

        return;
    }

    SIZE_T bytesRead = 0;
    char buffer[20] = {0};
    if (!ReadProcessMemory(sa2Handle, (LPCVOID)0x1DCFF0C, (LPVOID)buffer, (SIZE_T)20, &bytesRead) || bytesRead != 20)
    {
        CloseHandle(sa2Handle);
        sa2Handle = NULL;
        sa2PID = NULL;
        timeUntilNextProcessAttach = ATTACH_DELAY;
        return;
    }

    float camX;
    char* xptr = (char*)&camX;
    xptr[0] = buffer[0];
    xptr[1] = buffer[1];
    xptr[2] = buffer[2];
    xptr[3] = buffer[3];

    float camY;
    char* yptr = (char*)&camY;
    yptr[0] = buffer[4];
    yptr[1] = buffer[5];
    yptr[2] = buffer[6];
    yptr[3] = buffer[7];

    float camZ;
    char* zptr = (char*)&camZ;
    zptr[0] = buffer[8];
    zptr[1] = buffer[9];
    zptr[2] = buffer[10];
    zptr[3] = buffer[11];

    Global::gameCamera->eye.x = camX;
    Global::gameCamera->eye.y = camY;
    Global::gameCamera->eye.z = camZ;

    int pitch;
    memcpy(&pitch, &buffer[12], 4);
    
    int yaw;
    memcpy(&yaw, &buffer[16], 4);

    Global::gameCamera->yaw = -Maths::toDegrees(yaw);
    Global::gameCamera->pitch = -Maths::toDegrees(pitch);

    //0x019ED3FC = global position copy
    bytesRead = 0;
    unsigned long long masterobjptr = 0;
    if (!ReadProcessMemory(sa2Handle, (LPCVOID)0x01DEA6E0, (LPVOID)(&masterobjptr), (SIZE_T)4, &bytesRead) || bytesRead != 4)
    {
        CloseHandle(sa2Handle);
        sa2Handle = NULL;
        sa2PID = NULL;
        timeUntilNextProcessAttach = ATTACH_DELAY;
        return;
    }

    bytesRead = 0;
    unsigned long long chobj1 = 0;
    if (!ReadProcessMemory(sa2Handle, (LPCVOID)(masterobjptr+0x34), (LPVOID)(&chobj1), (SIZE_T)4, &bytesRead) || bytesRead != 4)
    {
        CloseHandle(sa2Handle);
        sa2Handle = NULL;
        sa2PID = NULL;
        timeUntilNextProcessAttach = ATTACH_DELAY;
        return;
    }

    bytesRead = 0;
    float sonicX = 0;
    if (!ReadProcessMemory(sa2Handle, (LPCVOID)(chobj1+0x14), (LPVOID)(&sonicX), (SIZE_T)4, &bytesRead) || bytesRead != 4)
    {
        CloseHandle(sa2Handle);
        sa2Handle = NULL;
        sa2PID = NULL;
        timeUntilNextProcessAttach = ATTACH_DELAY;
        return;
    }

    bytesRead = 0;
    float sonicY = 0;
    if (!ReadProcessMemory(sa2Handle, (LPCVOID)(chobj1+0x18), (LPVOID)(&sonicY), (SIZE_T)4, &bytesRead) || bytesRead != 4)
    {
        CloseHandle(sa2Handle);
        sa2Handle = NULL;
        sa2PID = NULL;
        timeUntilNextProcessAttach = ATTACH_DELAY;
        return;
    }

    bytesRead = 0;
    float sonicZ = 0;
    if (!ReadProcessMemory(sa2Handle, (LPCVOID)(chobj1+0x1C), (LPVOID)(&sonicZ), (SIZE_T)4, &bytesRead) || bytesRead != 4)
    {
        CloseHandle(sa2Handle);
        sa2Handle = NULL;
        sa2PID = NULL;
        timeUntilNextProcessAttach = ATTACH_DELAY;
        return;
    }

    bytesRead = 0;
    int bamsX = 0;
    if (!ReadProcessMemory(sa2Handle, (LPCVOID)(chobj1+0x08), (LPVOID)(&bamsX), (SIZE_T)4, &bytesRead) || bytesRead != 4)
    {
        CloseHandle(sa2Handle);
        sa2Handle = NULL;
        sa2PID = NULL;
        timeUntilNextProcessAttach = ATTACH_DELAY;
        return;
    }

    bytesRead = 0;
    int bamsY = 0;
    if (!ReadProcessMemory(sa2Handle, (LPCVOID)(chobj1+0x0C), (LPVOID)(&bamsY), (SIZE_T)4, &bytesRead) || bytesRead != 4)
    {
        CloseHandle(sa2Handle);
        sa2Handle = NULL;
        sa2PID = NULL;
        timeUntilNextProcessAttach = ATTACH_DELAY;
        return;
    }

    bytesRead = 0;
    int bamsZ = 0;
    if (!ReadProcessMemory(sa2Handle, (LPCVOID)(chobj1+0x10), (LPVOID)(&bamsZ), (SIZE_T)4, &bytesRead) || bytesRead != 4)
    {
        CloseHandle(sa2Handle);
        sa2Handle = NULL;
        sa2PID = NULL;
        timeUntilNextProcessAttach = ATTACH_DELAY;
        return;
    }

    //float sonicX;
    //char* sxptr = (char*)&sonicX;
    //sxptr[0] = buffer[0];
    //sxptr[1] = buffer[1];
    //sxptr[2] = buffer[2];
    //sxptr[3] = buffer[3];
    //
    //float sonicY;
    //char* syptr = (char*)&sonicY;
    //syptr[0] = buffer[4];
    //syptr[1] = buffer[5];
    //syptr[2] = buffer[6];
    //syptr[3] = buffer[7];
    //
    //float sonicZ;
    //char* szptr = (char*)&sonicZ;
    //szptr[0] = buffer[8];
    //szptr[1] = buffer[9];
    //szptr[2] = buffer[10];
    //szptr[3] = buffer[11];

    Global::gameCursor3D->setPosition(sonicX, sonicY, sonicZ);
    Global::gamePlayer->setPosition(sonicX, sonicY, sonicZ);
    Global::gamePlayer->setRotation(bamsX, -bamsY, bamsZ);
    Global::gamePlayer->updateTransformationMatrix();


    //make the score have a 1 at the end as a first
    // line of defence against cheaters :)
    char score;
    bytesRead = 0;
    if (!ReadProcessMemory(sa2Handle, (LPCVOID)0x0174B050, (LPVOID)(&score), (SIZE_T)1, &bytesRead) || bytesRead != 1)
    {
        CloseHandle(sa2Handle);
        sa2Handle = NULL;
        sa2PID = NULL;
        timeUntilNextProcessAttach = ATTACH_DELAY;
        return;
    }

    score = score | 0x1;
    SIZE_T bytesWritten = 0;
    if (!WriteProcessMemory(sa2Handle, (LPVOID)0x0174B050, (LPCVOID)(&score), (SIZE_T)1, &bytesWritten) || bytesWritten != 1)
    {
        CloseHandle(sa2Handle);
        sa2Handle = NULL;
        sa2PID = NULL;
        timeUntilNextProcessAttach = ATTACH_DELAY;
        return;
    }
}

void Global::teleportSA2PlayerToCursor3D()
{
    DWORD pid = getPIDByName("sonic2app.exe");

    if (pid == NULL)
    {
        return;
    }

    HANDLE handle = OpenProcess(PROCESS_VM_READ | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, false, pid);
    if (handle == NULL)
    {
        return;
    }

    SIZE_T bytesRead = 0;
    unsigned long long masterobjptr = 0;
    if (!ReadProcessMemory(handle, (LPCVOID)0x01DEA6E0, (LPVOID)(&masterobjptr), (SIZE_T)4, &bytesRead) || bytesRead != 4)
    {
        CloseHandle(handle);
        return;
    }

    if (masterobjptr <= 0x400000)
    {
        CloseHandle(handle);
        return;
    }

    bytesRead = 0;
    unsigned long long chobj1ptr = 0;
    if (!ReadProcessMemory(handle, (LPCVOID)(masterobjptr+0x34), (LPVOID)(&chobj1ptr), (SIZE_T)4, &bytesRead) || bytesRead != 4)
    {
        CloseHandle(handle);
        return;
    }

    if (chobj1ptr <= 0x400000)
    {
        CloseHandle(handle);
        return;
    }

    SIZE_T bytesWritten = 0;
    float sonicX = Global::gameCursor3D->position.x;
    if (!WriteProcessMemory(handle, (LPVOID)(chobj1ptr+0x14), (LPCVOID)(&sonicX), (SIZE_T)4, &bytesWritten) || bytesWritten != 4)
    {
        CloseHandle(handle);
        return;
    }

    bytesWritten = 0;
    float sonicY = Global::gameCursor3D->position.y+5;
    if (!WriteProcessMemory(handle, (LPVOID)(chobj1ptr+0x18), (LPCVOID)(&sonicY), (SIZE_T)4, &bytesWritten) || bytesWritten != 4)
    {
        CloseHandle(handle);
        return;
    }

    bytesWritten = 0;
    float sonicZ = Global::gameCursor3D->position.z;
    if (!WriteProcessMemory(handle, (LPVOID)(chobj1ptr+0x1C), (LPCVOID)(&sonicZ), (SIZE_T)4, &bytesWritten) || bytesWritten != 4)
    {
        CloseHandle(handle);
        return;
    }

    CloseHandle(handle);
}