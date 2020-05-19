#ifndef GUITEXTURE_H
#define GUITEXTURE_H

#include <glad/glad.h>
#include "../toolbox/vector.h"

class GuiTexture
{
private:
    GLuint textureID;
    Vector2f position;
    Vector2f size;
    float rotation;
    bool visible;

public:
    GuiTexture();

    //position is (0,0) = top left of screen, (1,1) is bottom right
    //Note that the image is drawn with its CENTER at position
    GuiTexture(GLuint textureID, Vector2f* position, Vector2f* size, float rotation);

    //position is (0,0) = top left of screen, (1,1) is bottom right
    //Note that the image is drawn with its CENTER at position
    GuiTexture(GLuint textureID, float posX, float posY, float sizeX, float sizeY, float rotation);

    GLuint getTexture();

    void setTexture(GLuint newTextureID);

    //WARNING: ONLY use this to read the position, do NOT use this to write to the position, use setX and setY
    Vector2f* getPosition();

    Vector2f* getSize();
    void setSize(Vector2f* newSize);
    void setSize(float newX, float newY);

    float getRotation();

    void setRotation(float newRotation);

    //position is (0,0) = top left of screen, (1,1) is bottom right
    //Note that the image is drawn with its CENTER at position
    void setX(float newX);

    //position is (0,0) = top left of screen, (1,1) is bottom right
    //Note that the image is drawn with its CENTER at position
    void setY(float newY);

    bool getVisible();

    void setVisible(bool newVisible);
};

#endif
