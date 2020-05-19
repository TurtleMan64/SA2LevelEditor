#ifndef MASTERRENDERER_H
#define MASTERRENDERER_H

class Entity;
class Matrix4f;
class Camera;
class ShaderProgram;
class EntityRenderer;
class TexturedModel;

#include <unordered_map>
#include <list>

class MasterRenderer
{
private:
    static ShaderProgram* shader;
    static EntityRenderer* renderer;

    static std::unordered_map<TexturedModel*, std::list<Entity*>> entitiesMap;
    static std::unordered_map<TexturedModel*, std::list<Entity*>> entitiesMapPass2;
    static std::unordered_map<TexturedModel*, std::list<Entity*>> entitiesMapPass3;
    static std::unordered_map<TexturedModel*, std::list<Entity*>> entitiesTransparentMap;

    static Matrix4f* projectionMatrix;

    static float VFOV; //Vertical fov
    static float HFOV; //Horizontal fov

    static float RED;
    static float GREEN;
    static float BLUE;

    static void prepare();
    static void prepareTransparentRender();
    static void prepareTransparentRenderDepthOnly();

public:
    static float DEFAULT_VFOV;
    static const float NEAR_PLANE; //0.5
    static const float FAR_PLANE; //15000

    static void render(Camera* camera);

    static void processEntity(Entity* entity);
    static void processEntityPass2(Entity* entity);
    static void processEntityPass3(Entity* entity);
    static void processTransparentEntity(Entity* entity);
         
    static void clearEntities();
    static void clearEntitiesPass2();
    static void clearEntitiesPass3();
    static void clearTransparentEntities();
         
    static void init();
    static void cleanUp();
         
    static void enableCulling();
    static void disableCulling();
         
    static void makeProjectionMatrix();
    static Matrix4f* getProjectionMatrix();

    static void setVFOV(float newVFOV);
    static float getVFOV();
    static float getHFOV();

    static float getNearPlane();
    static float getFarPlane();
};
#endif
