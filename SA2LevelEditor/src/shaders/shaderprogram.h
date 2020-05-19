#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

class Matrix4f;
class Camera;
class Light;
class Vector3f;

#include <glad/glad.h>


class ShaderProgram
{
private:
    GLuint programID;
    GLuint vertexShaderID;
    GLuint fragmentShaderID;

    int location_transformationMatrix;
    int location_projectionMatrix;
    int location_viewMatrix;
    int location_hasTransparency;
    int location_baseColour;
    int location_texOffX;
    int location_texOffY;
    int location_clipPlaneBehind;
    int location_mixFactor;
    int location_textureSampler2;

public:
    ShaderProgram(const char* vertexFilename, const char* fragmentFilename);

    void start();

    void stop();

    void cleanUp();

    void loadTransformationMatrix(Matrix4f*);

    void loadProjectionMatrix(Matrix4f*);

    void loadViewMatrix(Camera*);

    void loadTransparency(int transparency);

    void loadBaseColour(Vector3f* baseColour);

    void loadTextureOffsets(float offX, float offY);

    void loadClipPlaneBehind(float clipX, float clipY, float clipZ, float clipW);

    void loadMixFactor(float factor);

    void connectTextureUnits();

protected:
    void bindAttributes();

    void bindAttribute(int, const char*);

    void bindFragOutput(int attatchment, const char* variableName);

    void getAllUniformLocations();

    int getUniformLocation(const char*);

    void loadFloat(int, float);

    void loadInt(int, int);

    void loadVector(int, Vector3f*);

    void loadBoolean(int, bool);

    void loadMatrix(int, Matrix4f*);
};

#endif
