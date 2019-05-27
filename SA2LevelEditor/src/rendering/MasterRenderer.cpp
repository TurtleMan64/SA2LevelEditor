#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../entities/camera.h"
#include "../shaders/shaderprogram.h"
#include "../entities/entity.h"
#include "../models/texturedmodel.h"
#include "renderer.h"
#include "masterrenderer.h"
#include "../toolbox/maths.h"
#include "../toolbox/matrix.h"
#include "../main/main.h"
#include "../main/displaymanager.h"
//#include "../particles/particlemaster.h"

#include <iostream>
#include <list>
#include <unordered_map>
#include <stdexcept>

ShaderProgram* MasterRenderer::shader = nullptr;
EntityRenderer* MasterRenderer::renderer = nullptr;

std::unordered_map<TexturedModel*, std::list<Entity*>> MasterRenderer::entitiesMap;
std::unordered_map<TexturedModel*, std::list<Entity*>> MasterRenderer::entitiesMapPass2;
std::unordered_map<TexturedModel*, std::list<Entity*>> MasterRenderer::entitiesMapPass3;
std::unordered_map<TexturedModel*, std::list<Entity*>> MasterRenderer::entitiesTransparentMap;

Matrix4f* MasterRenderer::projectionMatrix = nullptr;

float MasterRenderer::DEFAULT_VFOV = 75.0f;
float MasterRenderer::VFOV = 0; //Vertical fov
float MasterRenderer::HFOV = 0; //Horizontal fov
const float MasterRenderer::NEAR_PLANE = 1.5f; //0.5 old value
const float MasterRenderer::FAR_PLANE = 60000; //15000 old value

#ifdef OBS_MODE
float MasterRenderer::RED   = 0.0f;
float MasterRenderer::GREEN = 1.0f;
float MasterRenderer::BLUE  = 0.0f;
#else
float MasterRenderer::RED   = 0.2f;
float MasterRenderer::GREEN = 0.2f;
float MasterRenderer::BLUE  = 0.2f;
#endif

void MasterRenderer::init()
{
	shader = new ShaderProgram("res/Shaders/entity/vertexShader.txt", "res/Shaders/entity/fragmentShader.txt"); INCR_NEW("ShaderProgram");
	projectionMatrix = new Matrix4f; INCR_NEW("Matrix4f");
	renderer = new EntityRenderer(shader, projectionMatrix); INCR_NEW("EntityRenderer");

    #ifdef OBS_MODE
    MasterRenderer::setVFOV(55.404052734375f);
    #else
    MasterRenderer::setVFOV(MasterRenderer::DEFAULT_VFOV);
    #endif

	MasterRenderer::makeProjectionMatrix();
	MasterRenderer::disableCulling();
}

void MasterRenderer::render(Camera* camera)
{
	prepare();
	shader->start();
	//shader->loadClipPlane(clipX, clipY, clipZ, clipW);

	//calc behind clipm plane based on camera
	Vector3f camDir = camera->calcForward();
	camDir.normalize();
	camDir.neg();
	Vector3f startPos(&camera->eye);
	//startPos = startPos + camDir.scaleCopy(-100);
	Vector4f plane = Maths::calcPlaneValues(&startPos, &camDir);
	shader->loadClipPlaneBehind(plane.x, plane.y, plane.z, plane.w);

	//RED = SkyManager::getFogRed();
	//GREEN = SkyManager::getFogGreen();
	//BLUE = SkyManager::getFogBlue();
	//shader->loadLight(Global::gameLightSun);
	shader->loadViewMatrix(camera);
	shader->connectTextureUnits();

	renderer->renderNEW(&entitiesMap);
	renderer->renderNEW(&entitiesMapPass2);
	renderer->renderNEW(&entitiesMapPass3);

	prepareTransparentRender();
	renderer->renderNEW(&entitiesTransparentMap);
	prepareTransparentRenderDepthOnly();
	renderer->renderNEW(&entitiesTransparentMap);

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	shader->stop();
}

void MasterRenderer::processEntity(Entity* entity)
{
	if (entity->visible == false)
	{
		return;
	}

	std::list<TexturedModel*>* modellist = entity->getModels();
	for (TexturedModel* entityModel : (*modellist))
	{
		std::list<Entity*>* list = &entitiesMap[entityModel];
		list->push_back(entity);
	}
}

void MasterRenderer::processEntityPass2(Entity* entity)
{
	if (entity->visible == false)
	{
		return;
	}

	std::list<TexturedModel*>* modellist = entity->getModels();
	for (TexturedModel* entityModel : (*modellist))
	{
		std::list<Entity*>* list = &entitiesMapPass2[entityModel];
		list->push_back(entity);
	}
}

void MasterRenderer::processEntityPass3(Entity* entity)
{
	if (entity->visible == false)
	{
		return;
	}

	std::list<TexturedModel*>* modellist = entity->getModels();
	for (TexturedModel* entityModel : (*modellist))
	{
		std::list<Entity*>* list = &entitiesMapPass3[entityModel];
		list->push_back(entity);
	}
}

void MasterRenderer::processTransparentEntity(Entity* entity)
{
	if (entity->visible == false)
	{
		return;
	}

	std::list<TexturedModel*>* modellist = entity->getModels();
	for (TexturedModel* entityModel : (*modellist))
	{
		std::list<Entity*>* list = &entitiesTransparentMap[entityModel];
		list->push_back(entity);
	}
}

void MasterRenderer::clearEntities()
{
	entitiesMap.clear();
}

void MasterRenderer::clearEntitiesPass2()
{
	entitiesMapPass2.clear();
}

void MasterRenderer::clearEntitiesPass3()
{
	entitiesMapPass3.clear();
}

void MasterRenderer::clearTransparentEntities()
{
	entitiesTransparentMap.clear();
}

void MasterRenderer::prepare()
{
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(true);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(RED, GREEN, BLUE, 1);

    if (Global::renderWithCulling)
    {
        MasterRenderer::enableCulling();
    }
    else
    {
        MasterRenderer::disableCulling();
    }
}

void MasterRenderer::prepareTransparentRender()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(false);

    if (Global::renderWithCulling)
    {
        MasterRenderer::enableCulling();
    }
    else
    {
        MasterRenderer::disableCulling();
    }
}

void MasterRenderer::prepareTransparentRenderDepthOnly()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(true);

    if (Global::renderWithCulling)
    {
        MasterRenderer::enableCulling();
    }
    else
    {
        MasterRenderer::disableCulling();
    }
}

void MasterRenderer::cleanUp()
{
	shader->cleanUp();
	delete shader; INCR_DEL("ShaderProgram");
	delete renderer; INCR_DEL("EntityRenderer");
	delete projectionMatrix; INCR_DEL("Matrix4f");
}

void MasterRenderer::enableCulling()
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

void MasterRenderer::disableCulling()
{
	glDisable(GL_CULL_FACE);
}

void MasterRenderer::makeProjectionMatrix()
{
	int displayWidth;
	int displayHeight;
	glfwGetWindowSize(DisplayManager::getWindow(), &displayWidth, &displayHeight);

	float aspectRatio = (float)displayWidth / (float)displayHeight;

	float y_scale = 1.0f / tanf(Maths::toRadians((VFOV) / 2.0f));
	float x_scale = y_scale / aspectRatio;

	float frustum_length = FAR_PLANE - NEAR_PLANE;

	projectionMatrix->m00 = x_scale;
	projectionMatrix->m11 = y_scale;
	projectionMatrix->m22 = -((FAR_PLANE + NEAR_PLANE) / frustum_length);
	projectionMatrix->m23 = -1;
	projectionMatrix->m32 = -((2 * NEAR_PLANE * FAR_PLANE) / frustum_length);
	projectionMatrix->m33 = 0;

	renderer->updateProjectionMatrix(projectionMatrix);

	//ParticleMaster::updateProjectionMatrix(projectionMatrix);
}

Matrix4f* MasterRenderer::getProjectionMatrix()
{
	return projectionMatrix;
}

void MasterRenderer::setVFOV(float newVFOV)
{
    int displayWidth;
	int displayHeight;
	glfwGetWindowSize(DisplayManager::getWindow(), &displayWidth, &displayHeight);

	float aspectRatio = (float)displayWidth / (float)displayHeight;

    float heightOfFrustrum = 2*getNearPlane()*tanf(Maths::toRadians(newVFOV/2));
    float widthOfFrustrum = aspectRatio*heightOfFrustrum;

    VFOV = newVFOV;
    HFOV = Maths::toDegrees(2*(atan2f(widthOfFrustrum/2, getNearPlane())));
}

float MasterRenderer::getVFOV()
{
	return VFOV;
}

float MasterRenderer::getHFOV()
{
    return HFOV;
}

float MasterRenderer::getNearPlane()
{
	return NEAR_PLANE;
}

float MasterRenderer::getFarPlane()
{
	return FAR_PLANE;
}
