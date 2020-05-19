#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cmath>
#include <fstream>
#include <string>
#include <cstring>
#include <algorithm>

#include "input.h"
#include "../main/main.h"
#include "maths.h"
#include "../toolbox/split.h"
#include "../toolbox/getline.h"
#include "../main/displaymanager.h"
#include <random>
#include <chrono>


//vars for use by us
double mousePreviousX = 0;
double mousePreviousY = 0;

//settings
bool freeMouse = true;

float mouseSensitivityX = 0.25f;
float mouseSensitivityY = 0.25f;

void Input::waitForInputs()
{
    glfwWaitEvents();

    double xpos, ypos;
    GLFWwindow* window = DisplayManager::getWindow();
    glfwGetCursorPos(window, &xpos, &ypos);

    if (freeMouse == false)
    {
        //Input::inputs.INPUT_X2 += (float)(mouseSensitivityX*(xpos - mousePreviousX));
        //Input::inputs.INPUT_Y2 += (float)(mouseSensitivityY*(ypos - mousePreviousY));
    }
    mousePreviousX = xpos;
    mousePreviousY = ypos;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}


void Input::init()
{
    //load sensitivity and button mappings from external file
    /*
    std::ifstream file("Settings/CameraSensitivity.ini");
    if (!file.is_open())
    {
        std::fprintf(stdout, "Error: Cannot load file 'Settings/CameraSensitivity.ini'\n");
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
                if (strcmp(lineSplit[0], "Mouse_X") == 0)
                {
                    mouseSensitivityX = std::stof(lineSplit[1], nullptr);
                }
                else if (strcmp(lineSplit[0], "Mouse_Y") == 0)
                {
                    mouseSensitivityY = std::stof(lineSplit[1], nullptr);
                }
                else if (strcmp(lineSplit[0], "Stick_X") == 0)
                {
                    //stickSensitivityX = std::stof(lineSplit[1], nullptr);
                }
                else if (strcmp(lineSplit[0], "Stick_Y") == 0)
                {
                    //stickSensitivityY = std::stof(lineSplit[1], nullptr);
                }
                else if (strcmp(lineSplit[0], "Triggers") == 0)
                {
                    //triggerSensitivity = std::stof(lineSplit[1], nullptr);
                }
            }

            free(lineSplit);
        }
        file.close();
    }
    */
}
