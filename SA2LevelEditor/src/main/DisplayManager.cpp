#include <Windows.h>
#include <commdlg.h>
#include <tchar.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <SOIL/SOIL.h>

#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <cstring>

#include "displaymanager.h"
#include "../toolbox/input.h"
#include "../main/main.h"
#include "../toolbox/split.h"
#include "../toolbox/getline.h"
#include "../rendering/masterrenderer.h"
#include "../entities/camera.h"
#include "../toolbox/maths.h"
#include "../toolbox/vector.h"
#include "../collision/collisionchecker.h"
#include "../entities/cursor3d.h"
#include "../entities/stagecollision.h"
#include "../entities/stagekillplanes.h"
#include "../entities/stagesky.h"
#include "../entities/stage.h"
#include "../loading/levelloader.h"

#include "../entities/GlobalObjects/ring.h"

// default settings
unsigned int DisplayManager::SCR_WIDTH = 1280;
unsigned int DisplayManager::SCR_HEIGHT = 720;
unsigned int DisplayManager::AA_SAMPLES = 0;
GLFWwindow* DisplayManager::glfwWindow = nullptr;

int DisplayManager::createDisplay()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Request an OpenGL 4.0 core context.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	loadDisplaySettings();
	loadGraphicsSettings();

	GLFWmonitor* monitor = nullptr;

	glfwWindowHint(GLFW_SAMPLES, AA_SAMPLES);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	unsigned int screenWidth  = SCR_WIDTH;
	unsigned int screenHeight = SCR_HEIGHT;


	//int count;
	//const GLFWvidmode* modes = glfwGetVideoModes(monitor, &count);

	//for (int i = 0; i < count; i++)
	//{
		//std::fprintf(stdout, "%s\n", modes[i].);
	//}

	// glfw window creation
	// --------------------
	glfwWindow = glfwCreateWindow(screenWidth, screenHeight, "SA2 Level Editor", monitor, nullptr);
	if (glfwWindow == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(glfwWindow);
	glfwSetFramebufferSizeCallback(glfwWindow, DisplayManager::framebuffer_size_callback);
	glfwSetWindowCloseCallback(glfwWindow, DisplayManager::window_close_callback);
    glfwSetCursorPosCallback(glfwWindow, DisplayManager::callbackCursorPosition);
    glfwSetScrollCallback(glfwWindow, DisplayManager::callbackMouseScroll);
    glfwSetMouseButtonCallback(glfwWindow, DisplayManager::callbackMouseClick);
    glfwSetKeyCallback(glfwWindow, DisplayManager::callbackKeyboard);

	GLFWimage icons[3];
	icons[0].pixels = SOIL_load_image("res/Images/Icon16.png", &icons[0].width, &icons[0].height, 0, SOIL_LOAD_RGBA);
	icons[1].pixels = SOIL_load_image("res/Images/Icon32.png", &icons[1].width, &icons[1].height, 0, SOIL_LOAD_RGBA);
	icons[2].pixels = SOIL_load_image("res/Images/Icon64.png", &icons[2].width, &icons[2].height, 0, SOIL_LOAD_RGBA);

    if (icons[0].pixels != nullptr &&
        icons[1].pixels != nullptr &&
        icons[2].pixels != nullptr)
    {
	    glfwSetWindowIcon(glfwWindow, 3, icons);
	    SOIL_free_image_data(icons[0].pixels);
	    SOIL_free_image_data(icons[1].pixels);
	    SOIL_free_image_data(icons[2].pixels);
    }

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	std::fprintf(stdout, "Renderer:       %s\n", glGetString(GL_RENDERER));
	std::fprintf(stdout, "Vendor:         %s\n", glGetString(GL_VENDOR));
	std::fprintf(stdout, "OpenGL version: %s\n", glGetString(GL_VERSION));
	std::fprintf(stdout, "GLSL version:   %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	glfwSwapInterval(1); //1 = vsync. 0 = off. 2 = half monitor refresh rate
	glEnable(GL_MULTISAMPLE);

	//Center the window
	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	int monitorWidth = mode->width;
	int monitorHeight = mode->height;

	if ((int)SCR_WIDTH  <= monitorWidth && 
		(int)SCR_HEIGHT <= monitorHeight)
	{
		int xpos = monitorWidth/2  - ((int)SCR_WIDTH)/2;
		int ypos = monitorHeight/2 - ((int)SCR_HEIGHT)/2;

		glfwSetWindowPos(glfwWindow, xpos, ypos);
	}

	//glfwGetWindowAttrib(window, GLFW_SAMPLES);
	//std::fprintf(stdout, "samples:   %d\n", glfwGetWindowAttrib(window, GLFW_SAMPLES));

	//float aniso = 0.0f;
	//glGetFloatv(GL_MAX_TEXTURE_LOD_BIAS, &aniso);
	//std::fprintf(stdout, "max lod bias:   %f\n", aniso);

	//To check what extensions are avalible 
	//int ext_cnt;
	//glGetIntegerv(GL_NUM_EXTENSIONS, &ext_cnt);
	//for (int i = 0; i < ext_cnt; i++)
	{
		//std::fprintf(stdout, "extensions:   %s\n", glGetStringi(GL_EXTENSIONS, i));
	}

	//Master_makeProjectionMatrix();

	return 0;
}

void DisplayManager::updateDisplay()
{
	glfwSwapBuffers(glfwWindow);
}

void DisplayManager::closeDisplay()
{
	glfwDestroyWindow(glfwWindow);
	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
}

int DisplayManager::displayWantsToClose()
{
	return glfwWindowShouldClose(glfwWindow);
}

GLFWwindow* DisplayManager::getWindow()
{
	return glfwWindow;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void DisplayManager::framebuffer_size_callback(GLFWwindow* /*windowHandle*/, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
	MasterRenderer::makeProjectionMatrix();
    Global::redrawWindow = true;
}

double DisplayManager::prevXPos = 0;
double DisplayManager::prevYPos = 0;
void DisplayManager::callbackCursorPosition(GLFWwindow* window, double xpos, double ypos)
{
    float xDiff = (float)(xpos - prevXPos);
    float yDiff = (float)(ypos - prevYPos);
    DisplayManager::prevXPos = xpos;
    DisplayManager::prevYPos = ypos;

    int stateMouse = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);
    if (stateMouse == GLFW_PRESS) //rotate the camera if you hold middle click
    {
        if (Global::isHoldingShift && Global::isHoldingAlt) //pan based on distance from 3d cursor
        {
            Vector3f distFromCursor = Global::gameCursor3D->position - Global::gameCamera->eye;
            float PAN_SPEED = 0.005f*distFromCursor.length();

            Vector3f camRight = Global::gameCamera->calcRight();
            Vector3f camUp = Global::gameCamera->calcUp();
            camRight.normalize();
            camUp.normalize();

            Vector3f offset = camUp.scaleCopy(-yDiff*PAN_SPEED) + camRight.scaleCopy(-xDiff*PAN_SPEED);
            Global::gameCamera->eye = Global::gameCamera->eye + offset;
        }
        else if (Global::isHoldingShift)  //pan the camera if you hold middle click and shift
        {
            const float PAN_SPEED = 0.5f;

            Vector3f camRight = Global::gameCamera->calcRight();
            Vector3f camUp = Global::gameCamera->calcUp();
            camRight.normalize();
            camUp.normalize();

            Vector3f offset = camUp.scaleCopy(-yDiff*PAN_SPEED) + camRight.scaleCopy(-xDiff*PAN_SPEED);
            Global::gameCamera->eye = Global::gameCamera->eye + offset;
        }
        else if (Global::isHoldingAlt)  //rotate the camera around the 3d cursor
        {
            const float ROTATE_SPEED = 0.2f;
            float rotateAmountRadiansYaw   = Maths::toRadians(xDiff*ROTATE_SPEED);
            float rotateAmountRadiansPitch = Maths::toRadians(yDiff*ROTATE_SPEED);

            float pitchBefore = Maths::toRadians(Global::gameCamera->pitch);
            float pitchAfter = pitchBefore + rotateAmountRadiansPitch;
            pitchAfter = fmaxf(pitchAfter, Maths::toRadians(-89.99f));
            pitchAfter = fminf(pitchAfter, Maths::toRadians( 89.99f));
            rotateAmountRadiansPitch = pitchAfter-pitchBefore;

            Global::gameCamera->yaw   += Maths::toDegrees(rotateAmountRadiansYaw);
            Global::gameCamera->pitch += Maths::toDegrees(rotateAmountRadiansPitch);

            Vector3f diff = Global::gameCamera->eye - Global::gameCursor3D->position;

            Vector3f yAxis(0, -1, 0); //rotate horizontally
            diff = Maths::rotatePoint(&diff, &yAxis, rotateAmountRadiansYaw);

            Vector3f perpen = yAxis.cross(&diff); //rotate vertically
            diff = Maths::rotatePoint(&diff, &perpen, rotateAmountRadiansPitch);

            Global::gameCamera->eye = Global::gameCursor3D->position + diff;
        }
        else  //rotate the camera if you hold middle click
        {
            const float ROTATE_SPEED = 0.2f;
            Global::gameCamera->yaw += xDiff*ROTATE_SPEED;

            float pitchBefore = Global::gameCamera->pitch;
            float pitchAfter = pitchBefore+yDiff*ROTATE_SPEED;

            pitchAfter = fmaxf(pitchAfter, -89.99f);
            pitchAfter = fminf(pitchAfter,  89.99f);

            Global::gameCamera->pitch = pitchAfter;
        }

        Global::redrawWindow = true;
    }

    if ((Global::isHoldingX || Global::isHoldingY || Global::isHoldingZ) && Global::selectedSA2Object != nullptr)
    {
        //rotate objects
        if (Global::isHoldingClickRight)
        {
            float ROT_SPEED = 0.07f;

            if (Global::isHoldingShift)
            {
                ROT_SPEED = 3.0f;
            }

            if (Global::isHoldingX)
            {
                Global::selectedSA2Object->rotationX -= (int)std::round((yDiff)*ROT_SPEED);
            }
            if (Global::isHoldingY)
            {
                Global::selectedSA2Object->rotationY -= (int)std::round((xDiff)*ROT_SPEED);
            }
            if (Global::isHoldingZ)
            {
                Global::selectedSA2Object->rotationZ -= (int)std::round((xDiff)*ROT_SPEED);
            }
        }
        else //move objects
        {
            float SLIDE_SPEED = 0.005f;

            if (Global::isHoldingShift)
            {
                SLIDE_SPEED = 0.5f;
            }

            if (Global::isHoldingX)
            {
                Global::selectedSA2Object->position.x += (xDiff)*SLIDE_SPEED;
            }
            if (Global::isHoldingY)
            {
                Global::selectedSA2Object->position.y -= (yDiff)*SLIDE_SPEED;
            }
            if (Global::isHoldingZ)
            {
                Global::selectedSA2Object->position.z += (yDiff)*SLIDE_SPEED;
            }
        }

        Global::selectedSA2Object->updateEditorWindows();
        Global::redrawWindow = true;
    }
}

void DisplayManager::callbackMouseScroll(GLFWwindow* /*window*/, double /*xoffset*/, double yoffset)
{
    if (Global::isHoldingAlt)
    {
        //move the camera forward or back based on distance from 3d cursor
        Vector3f camDiff = Global::gameCursor3D->position - Global::gameCamera->eye;
        camDiff.scale(0.1f*(float)yoffset);
        if (camDiff.length() > 0.16f || yoffset < 0)
        {
            Global::gameCamera->eye = Global::gameCamera->eye + camDiff;
        }
    }
    else
    {
        //move the camera forward or back at constant rate
        const float MOVE_SPEED = 40.0f;
        Vector3f camDir = Global::gameCamera->calcForward();
        camDir.normalize();
        Vector3f moveOffset = camDir.scaleCopy(((float)yoffset)*MOVE_SPEED);
        Global::gameCamera->eye = Global::gameCamera->eye + moveOffset;
    }

    Global::redrawWindow = true;
}

void DisplayManager::callbackMouseClick(GLFWwindow* window, int button, int action, int /*mods*/)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        Vector3f clickDir = Maths::calcWorldSpaceDirectionVectorFromScreenSpaceCoords((float)xpos, (float)ypos);
        clickDir.setLength(100);

        Vector3f checkEnd(&Global::gameCamera->eye);
        checkEnd = checkEnd + clickDir.scaleCopy(10000);
        if (CollisionChecker::checkCollision(&Global::gameCamera->eye, &checkEnd))
        {
            Global::gameCursor3D->setPosition(CollisionChecker::getCollidePosition());
        }
        else
        {
            Vector3f ringpos = Global::gameCamera->eye + clickDir;
            Global::gameCursor3D->setPosition(&ringpos);
        }

        Global::redrawWindow = true;
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (action == GLFW_PRESS)
        {
            Global::isHoldingClickRight = true;
        }
        else if (action == GLFW_RELEASE)
        {
            Global::isHoldingClickRight = false;
        }
    }
}

void DisplayManager::callbackKeyboard(GLFWwindow* /*window*/, int key, int /*scancode*/, int action, int mods)
{
    switch (key)
    {
        case GLFW_KEY_L:
            if (action == GLFW_PRESS)
            {
                Global::shouldLoadNewLevel = true;
            }
            break;

        case GLFW_KEY_E:
            if (action == GLFW_PRESS)
            {
                Global::shouldExportLevel = true;
            }
            break;

        case GLFW_KEY_X:
            if (action == GLFW_PRESS)
            {
                Global::isHoldingX = true;
                glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
            else if (action == GLFW_RELEASE)
            {
                Global::isHoldingX = false;
                if (!Global::isHoldingY && !Global::isHoldingZ)
                {
                    glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                }
            }
            break;

        case GLFW_KEY_Y:
            if (action == GLFW_PRESS)
            {
                Global::isHoldingY = true;
                glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
            else if (action == GLFW_RELEASE)
            {
                Global::isHoldingY = false;
                if (!Global::isHoldingX && !Global::isHoldingZ)
                {
                    glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                }
            }
            break;

        case GLFW_KEY_Z:
            if (action == GLFW_PRESS)
            {
                Global::isHoldingZ = true;
                glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
            else if (action == GLFW_RELEASE)
            {
                Global::isHoldingZ = false;
                if (!Global::isHoldingX && !Global::isHoldingY)
                {
                    glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                }
            }
            break;

        case GLFW_KEY_LEFT_SHIFT:
        case GLFW_KEY_RIGHT_SHIFT:
            if (action == GLFW_PRESS)
            {
                Global::isHoldingShift = true;
            }
            else if (action == GLFW_RELEASE)
            {
                Global::isHoldingShift = false;
            }
            break;

        case GLFW_KEY_LEFT_ALT:
        case GLFW_KEY_RIGHT_ALT:
            if (action == GLFW_PRESS)
            {
                Global::isHoldingAlt = true;
            }
            else if (action == GLFW_RELEASE)
            {
                Global::isHoldingAlt = false;
            }
            break;

        case GLFW_KEY_DELETE:
            if (action == GLFW_PRESS)
            {
                if (Global::selectedSA2Object != nullptr)
                {
                    Global::deleteEntity(Global::selectedSA2Object);

                    if (Global::selectedSA2Object->lvlLineNum >= 0)
                    {
                        LevelLoader::lvlFile[Global::selectedSA2Object->lvlLineNum] = "";
                    }

                    Global::selectedSA2Object->cleanUp();
                    Global::selectedSA2Object = nullptr;
                    Global::redrawWindow = true;
                    Global::resetObjectWindow();
                }
            }
            break;

        case GLFW_KEY_D:
            if (action == GLFW_PRESS && (mods & 0x0002))
            {
                if (Global::selectedSA2Object != nullptr)
                {
                    char data[32] = {0};
                    Global::selectedSA2Object->fillData(&data[0]);

                    SA2Object* newObject = LevelLoader::newSA2Object(Global::levelID, Global::selectedSA2Object->ID, &data[0], false);
                    if (newObject != nullptr)
                    {
                        newObject->position.y += 10;
                        Global::addEntity(newObject);
                        Global::selectedSA2Object = newObject;
                        Global::selectedSA2Object->updateEditorWindows();
                        Global::redrawWindow = true;
                    }
                }
            }
            break;

        case GLFW_KEY_KP_7:
            if (action == GLFW_PRESS)
            {
                Global::gameCamera->yaw = 0.0f;
                Global::gameCamera->pitch = 89.99f;
                Global::redrawWindow = true;
            }
            break;

        case GLFW_KEY_KP_4:
            if (action == GLFW_PRESS)
            {
                Global::gameCamera->yaw -= 22.5f;
                Global::redrawWindow = true;
            }
            break;

        case GLFW_KEY_KP_6:
            if (action == GLFW_PRESS)
            {
                Global::gameCamera->yaw += 22.5f;
                Global::redrawWindow = true;
            }
            break;

        default:
            break;
    }
}

void DisplayManager::window_close_callback(GLFWwindow* /*windowHandle*/)
{
	Global::gameState = STATE_EXITING;
}

void DisplayManager::loadDisplaySettings()
{
	std::ifstream file("Settings/DisplaySettings.ini");
	if (!file.is_open())
	{
		std::fprintf(stdout, "Error: Cannot load file 'Settings/DisplaySettings.ini'\n");
		file.close();
	}
	else
	{
		std::string line;

		while (!file.eof())
		{
			getlineSafe(file, line);

			char lineBuf[512];
			memcpy(lineBuf, line.c_str(), line.size()+1);

			int splitLength = 0;
			char** lineSplit = split(lineBuf, ' ', &splitLength);

			if (splitLength == 2)
			{
				if (strcmp(lineSplit[0], "Width") == 0)
				{
					SCR_WIDTH = std::stoi(lineSplit[1], nullptr, 10);
				}
				else if (strcmp(lineSplit[0], "Height") == 0)
				{
					SCR_HEIGHT = std::stoi(lineSplit[1], nullptr, 10);
				}
			}
			free(lineSplit);
		}
		file.close();
	}
}

void DisplayManager::loadGraphicsSettings()
{
	std::ifstream file("Settings/GraphicsSettings.ini");
	if (!file.is_open())
	{
		std::fprintf(stdout, "Error: Cannot load file 'Settings/GraphicsSettings.ini'\n");
		file.close();
	}
	else
	{
		std::string line;

		while (!file.eof())
		{
			getlineSafe(file, line);

			char lineBuf[512];
			memcpy(lineBuf, line.c_str(), line.size()+1);

			int splitLength = 0;
			char** lineSplit = split(lineBuf, ' ', &splitLength);

			if (splitLength == 2)
			{
				if (strcmp(lineSplit[0], "FOV") == 0)
				{
                    MasterRenderer::DEFAULT_VFOV = std::stof(lineSplit[1], nullptr);
				}
				else if (strcmp(lineSplit[0], "Anti-Aliasing_Samples") == 0)
				{
					AA_SAMPLES = std::stoi(lineSplit[1], nullptr, 10);
				}
			}
			free(lineSplit);
		}
		file.close();
	}
}

Vector2f DisplayManager::getResolution()
{
    return Vector2f((float)SCR_WIDTH, (float)SCR_HEIGHT);
}
