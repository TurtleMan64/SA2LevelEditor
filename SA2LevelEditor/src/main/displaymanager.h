#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H

#include <GLFW/glfw3.h>


class DisplayManager
{
private:
    static unsigned int SCR_WIDTH;
    static unsigned int SCR_HEIGHT;
    static unsigned int AA_SAMPLES;

    static GLFWwindow* globalWindow;

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void window_close_callback(GLFWwindow* window);

    static double prevXPos;
    static double prevYPos;
    static void callbackCursorPosition(GLFWwindow* window, double xpos, double ypos);

    static void callbackMouseScroll(GLFWwindow* window, double xoffset, double yoffset);

    static void loadDisplaySettings();
    static void loadGraphicsSettings();

public:
    static int createDisplay();
    static void updateDisplay();
    static void closeDisplay();
    static int displayWantsToClose();
    static GLFWwindow* getWindow();
};

#endif
