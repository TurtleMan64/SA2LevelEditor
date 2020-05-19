#include <glad/glad.h>

#include "entity.h"
#include "../toolbox/vector.h"
#include "cursor3d.h"
#include "../models/texturedmodel.h"
#include "../main/main.h"
#include "../main/displaymanager.h"
#include "../guis/guitexture.h"
#include "../loading/loader.h"
#include "../guis/guimanager.h"
#include "../toolbox/maths.h"

#include <list>

GuiTexture* Cursor3D::textureCursor3D = nullptr;

Cursor3D::Cursor3D()
{
    position.set(0,0,0);
    visible = false;
    if (Cursor3D::textureCursor3D == nullptr)
    {
        textureCursor3D = new GuiTexture(Loader::loadTextureNoInterpolation("res/Images/Cursor3D.png"), -1, -1, 1, 1, 0); INCR_NEW("GuiTexture")
        textureCursor3D->setVisible(true);
        GuiManager::addGuiToRender(Cursor3D::textureCursor3D);
    }
}

void Cursor3D::step()
{
    Vector2f windowSize = DisplayManager::getResolution();

    float px = 1.0f/(windowSize.x); //1 pixel in x dimension
    float py = 1.0f/(windowSize.y); //1 pixel in y dimension

    textureCursor3D->setSize(px*64, py*64);

    Vector2f screenPoint = Maths::calcScreenCoordsOfWorldPoint(&position);
    textureCursor3D->getPosition()->x = screenPoint.x;
    textureCursor3D->getPosition()->y = screenPoint.y;
}

std::list<TexturedModel*>* Cursor3D::getModels()
{
    return nullptr;
}

void Cursor3D::loadStaticModels()
{
    
}

void Cursor3D::deleteStaticModels()
{
    
}
