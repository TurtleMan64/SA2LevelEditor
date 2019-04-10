#include <glad/glad.h>

#include "renderer.h"
#include "../entities/entity.h"
#include "../shaders/shaderprogram.h"
#include "../toolbox/matrix.h"
#include "../toolbox/vector.h"
#include "../toolbox/maths.h"
#include "../models/texturedmodel.h"
#include "../main/main.h"

#include <iostream>
#include <unordered_map>
#include <list>

EntityRenderer::EntityRenderer(ShaderProgram* shader, Matrix4f* projectionMatrix)
{
	shader->start();
	shader->loadProjectionMatrix(projectionMatrix);
	shader->stop();
	this->shader = shader;
}

void EntityRenderer::renderNEW(std::unordered_map<TexturedModel*, std::list<Entity*>>* entitiesMap)
{
	//clockTime = Global::gameClock;

	for (auto entry : (*entitiesMap))
	{
		prepareTexturedModel(entry.first);
		std::list<Entity*>* entityList = &entry.second;

		for (Entity* entity : (*entityList))
		{
			prepareInstance(entity);
			glDrawElements(GL_TRIANGLES, (entry.first)->getRawModel()->getVertexCount(), GL_UNSIGNED_INT, 0);
		}
		unbindTexturedModel();
	}
}

void EntityRenderer::prepareTexturedModel(TexturedModel* model)
{
	RawModel* rawModel = model->getRawModel();
	glBindVertexArray(rawModel->getVaoID());
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	ModelTexture* texture = model->getTexture();
	//if (texture->getHasTransparency() != 0)
	{
		//Master_disableCulling();
	}
	//else
	{
		//Master_enableCulling();
	}
	shader->loadTransparency(texture->hasTransparency);
	shader->loadTextureOffsets(clockTime * (texture->scrollX), clockTime * (texture->scrollY));
    shader->loadMixFactor(texture->mixFactor());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->getID());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture->getID2());
}

void EntityRenderer::unbindTexturedModel()
{
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
	glBindVertexArray(0);
}

void EntityRenderer::prepareInstance(Entity* entity)
{
	shader->loadTransformationMatrix(entity->getTransformationMatrix());
	shader->loadBaseColour(&entity->baseColour);
}

void EntityRenderer::updateProjectionMatrix(Matrix4f* projectionMatrix)
{
	shader->start();
	shader->loadProjectionMatrix(projectionMatrix);
	shader->stop();
}
