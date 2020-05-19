#include <list>

#include "guitexture.h"
#include "guimanager.h"
#include "guirenderer.h"

std::list<GuiTexture*> GuiManager::guisToRender;

void GuiManager::init()
{
    GuiRenderer::init();
}

void GuiManager::renderAll()
{
    GuiRenderer::render(&GuiManager::guisToRender);
}

void GuiManager::addGuiToRender(GuiTexture* newImage)
{
    GuiManager::guisToRender.push_back(newImage);
}

void GuiManager::removeGui(GuiTexture* imageToRemove)
{
    GuiManager::guisToRender.remove(imageToRemove);
}

void GuiManager::clearGuisToRender()
{
    GuiManager::guisToRender.clear();
}
