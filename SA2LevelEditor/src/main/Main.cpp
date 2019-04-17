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
#include "../entities/GlobalObjects/ring.h"
#include "../entities/unknown.h"

#ifdef _WIN32
#include <windows.h>
#include <commdlg.h>
#include <tchar.h>
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
Camera*          Global::gameCamera          = nullptr;
Stage*           Global::gameStage           = nullptr;
StageCollision*  Global::gameStageCollision  = nullptr;
StageKillplanes* Global::gameStageKillplanes = nullptr;
SkySphere*       Global::gameSkySphere       = nullptr;
Cursor3D*        Global::gameCursor3D        = nullptr;

std::string Global::dirSA2Root = "C:/Program Files (x86)/Steam/steamapps/common/Sonic Adventure 2";
std::string Global::dirProgRoot = "";

int Global::countNew = 0;
int Global::countDelete = 0;
int Global::gameState = 0;
int Global::levelID = 0;
bool Global::shouldLoadNewLevel = false;

int Global::gameMissionNumber = 0;
std::unordered_map<std::string, std::string> Global::levelFileMap;
std::unordered_map<int, std::string> Global::levelIDMap;

HWND Global::mainWindow = nullptr;
std::vector<HWND> Global::windowLabels;
std::vector<HWND> Global::windowValues;
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

void addMenus(HWND window)
{
    HMENU menu = CreateMenu();
    HMENU menuFile = CreateMenu();

    AppendMenu(menuFile, MF_STRING, 1, "Load Objects from SET Files");
    AppendMenu(menuFile, MF_STRING, 2, "Export Objects to SET Files");
    AppendMenu(menuFile, MF_SEPARATOR, 0, "");
    AppendMenu(menuFile, MF_STRING, 3, "Exit");

    AppendMenu(menu, MF_POPUP, (UINT_PTR)menuFile, "File");
    AppendMenu(menu, MF_STRING, 4, "Help");

    SetMenu(window, menu);
}

void addControls(HWND window)
{
    //CreateWindowW(L"static", L"Enter text here:", WS_VISIBLE | WS_CHILD | WS_BORDER | SS_CENTER, 200, 100, 100, 50, window, NULL, NULL, NULL);
    //CreateWindowW(L"Edit", L"...", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL, 200, 152, 100, 50, window, NULL, NULL, NULL);
    int entryHOffset = 60;
    int pad = 2;

    int entryHeight = 20;
    int entryWidth = 80;
    Global::windowLabels.push_back(CreateWindowW(L"STATIC", L"ID",        WS_VISIBLE | WS_CHILD | WS_BORDER, 0+pad,  0*entryHOffset+( 1*pad), entryWidth, entryHeight, window, NULL, NULL, NULL));
    Global::windowLabels.push_back(CreateWindowW(L"STATIC", L"Name",      WS_VISIBLE | WS_CHILD | WS_BORDER, 0+pad,  1*entryHOffset+( 2*pad), entryWidth, entryHeight, window, NULL, NULL, NULL));
    Global::windowLabels.push_back(CreateWindowW(L"STATIC", L"PositionX", WS_VISIBLE | WS_CHILD | WS_BORDER, 0+pad,  2*entryHOffset+( 3*pad), entryWidth, entryHeight, window, NULL, NULL, NULL));
    Global::windowLabels.push_back(CreateWindowW(L"STATIC", L"PositionY", WS_VISIBLE | WS_CHILD | WS_BORDER, 0+pad,  3*entryHOffset+( 4*pad), entryWidth, entryHeight, window, NULL, NULL, NULL));
    Global::windowLabels.push_back(CreateWindowW(L"STATIC", L"PositionZ", WS_VISIBLE | WS_CHILD | WS_BORDER, 0+pad,  4*entryHOffset+( 5*pad), entryWidth, entryHeight, window, NULL, NULL, NULL));
    Global::windowLabels.push_back(CreateWindowW(L"STATIC", L"RotationX", WS_VISIBLE | WS_CHILD | WS_BORDER, 0+pad,  5*entryHOffset+( 6*pad), entryWidth, entryHeight, window, NULL, NULL, NULL));
    Global::windowLabels.push_back(CreateWindowW(L"STATIC", L"RotationY", WS_VISIBLE | WS_CHILD | WS_BORDER, 0+pad,  6*entryHOffset+( 7*pad), entryWidth, entryHeight, window, NULL, NULL, NULL));
    Global::windowLabels.push_back(CreateWindowW(L"STATIC", L"RotationZ", WS_VISIBLE | WS_CHILD | WS_BORDER, 0+pad,  7*entryHOffset+( 8*pad), entryWidth, entryHeight, window, NULL, NULL, NULL));
    Global::windowLabels.push_back(CreateWindowW(L"STATIC", L"Variable1", WS_VISIBLE | WS_CHILD | WS_BORDER, 0+pad,  8*entryHOffset+( 9*pad), entryWidth, entryHeight, window, NULL, NULL, NULL));
    Global::windowLabels.push_back(CreateWindowW(L"STATIC", L"Variable2", WS_VISIBLE | WS_CHILD | WS_BORDER, 0+pad,  9*entryHOffset+(10*pad), entryWidth, entryHeight, window, NULL, NULL, NULL));
    Global::windowLabels.push_back(CreateWindowW(L"STATIC", L"Variable3", WS_VISIBLE | WS_CHILD | WS_BORDER, 0+pad, 10*entryHOffset+(11*pad), entryWidth, entryHeight, window, NULL, NULL, NULL));

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
        std::fprintf(stdout, "wp = %d\n", (int)wp);
        switch (wp)
        {
        case 1: Global::shouldLoadNewLevel = true; break;
        case 2: break;
        case 3: break;
        case 4: MessageBox(NULL, "Load the U and S setfile in and the rest of the stage models will load automatically. Use C, V, and B keys to toggle visibility of the killplanes/collision/stage.", "Help", MB_OK); break;
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

    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
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

    //MSG msg = {0};

    //while (GetMessage(&msg, NULL, NULL, NULL))
    //{
    //    TranslateMessage(&msg);
    //    DispatchMessage(&msg);
    //}

    return 0;
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

	//Global::fontVipnagorgialla = new FontType(Loader::loadTexture("res/Fonts/vipnagorgialla.png"), "res/Fonts/vipnagorgialla.fnt"); INCR_NEW("FontType");

	//TextMaster::init();

	GuiManager::init();
    GuiTextureResources::loadGuiTextures();

    //this cursor never gets deleted
    Cursor3D cursor;
    Global::gameCursor3D = &cursor;

	//if (Global::renderParticles)
	{
		//ParticleResources::loadParticles();
	}

	//GuiTextureResources::loadGuiTextures();

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

	//This sky sphere never gets deleted.
	//SkySphere skySphere;
	//Global::gameSkySphere = &skySphere;

	//ParticleMaster::init(Master_getProjectionMatrix());

    //load all global object models
    RING::loadStaticModels();
    Unknown::loadStaticModels();


	glfwSetTime(0);

	int frameCount = 0;
	double previousTime = 0;



    //LevelLoader::loadLevel(3, "C:\\Program Files (x86)\\Steam\\steamapps\\common\\Sonic Adventure 2\\resource\\gd_PC");
    //Global::shouldLoadNewLevel = true;

	Global::gameState = STATE_RUNNING;

	while (Global::gameState != STATE_EXITING && DisplayManager::displayWantsToClose() == 0)
	{
		frameCount++;
		timeNew = glfwGetTime();
		dt = (float)(timeNew - timeOld);
		dt = std::fminf(dt, 0.04f); //Anything lower than 25fps will slow the gameplay down

		timeOld = timeNew;

		Input::waitForInputs();

        if (Global::shouldLoadNewLevel)
        {
            Global::shouldLoadNewLevel = false;
            LevelLoader::promptUserForLevel();
        }

        if (!Global::redrawWindow)
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
				//skySphere.step();
                //ModelTexture::updateAnimations(dt);
				Global::gameCamera->refresh();
                Global::gameCursor3D->step();
                Global::gameStage->step();
                Global::gameStageCollision->step();
                Global::gameStageKillplanes->step();
				//if (Global::renderParticles)
				{
					//ParticleMaster::update(Global::gameCamera);
				}
				//Global::gameClock+=dt;
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
		//MasterRenderer::processEntity(&skySphere);

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
	//TextMaster::cleanUp();
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
