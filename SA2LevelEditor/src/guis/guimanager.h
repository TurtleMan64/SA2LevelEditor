#ifndef GUIMANAGER_H
#define GUIMANAGER_H

class GuiTexture;

#include <list>

class GuiManager
{
private:
	static std::list<GuiTexture*> guisToRender;

public:
	static void init();

	static void renderAll();

	static void addGuiToRender(GuiTexture* newImage);

	static void removeGui(GuiTexture* newImage);

	static void clearGuisToRender();
};

#endif
