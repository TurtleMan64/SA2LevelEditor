#include "guitexture.h"

#include <glad/glad.h>
#include "../toolbox/vector.h"

GuiTexture::GuiTexture()
{
	
}

GuiTexture::GuiTexture(GLuint textureID, Vector2f* position, Vector2f* size, float rotation)
{
	this->textureID = textureID;
	this->position.x = (position->x*2.0f)-1;
	this->position.y = -((position->y*2.0f)-1);
	this->size.set(size);
	this->visible = true;
	this->rotation = rotation;
}

GuiTexture::GuiTexture(GLuint textureID, float posX, float posY, float sizeX, float sizeY, float rotation)
{
	this->textureID = textureID;
	this->position.x = (posX*2.0f)-1;
	this->position.y = -((posY*2.0f)-1);
	this->size.x = sizeX;
    this->size.y = sizeY;
	this->visible = true;
	this->rotation = rotation;
}

GLuint GuiTexture::getTexture()
{
	return textureID;
}

Vector2f* GuiTexture::getPosition()
{
	return &position;
}

void GuiTexture::setTexture(GLuint newTextureID)
{
	textureID = newTextureID;
}

void GuiTexture::setX(float newX)
{
	position.x = (newX*2.0f)-1;
}

void GuiTexture::setY(float newY)
{
	position.y = -((newY*2.0f)-1);
}

bool GuiTexture::getVisible()
{
	return visible;
}

void GuiTexture::setVisible(bool newVisible)
{
	visible = newVisible;
}

Vector2f* GuiTexture::getSize()
{
    return &size;
}

void GuiTexture::setSize(Vector2f* newSize)
{
    size.set(newSize);
}

void GuiTexture::setSize(float newX, float newY)
{
    size.x = newX;
    size.y = newY;
}

float GuiTexture::getRotation()
{
	return rotation;
}

void GuiTexture::setRotation(float newRotation)
{
	rotation = newRotation;
}
