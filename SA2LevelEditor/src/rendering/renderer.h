#ifndef RENDERER_H
#define RENDERER_H

class ShaderProgram;
class Entity;
class Matrix4f;
class TexturedModel;

#include <unordered_map>
#include <list>
#include <vector>


class EntityRenderer
{
private:
	float clockTime;
	ShaderProgram* shader;

	void prepareTexturedModel(TexturedModel* model);

	void unbindTexturedModel();

	void prepareInstance(Entity* entity);

public:
	EntityRenderer(ShaderProgram* shader, Matrix4f* projectionMatrix);

	void renderNEW(std::unordered_map<TexturedModel*, std::list<Entity*>>* entities);

	void updateProjectionMatrix(Matrix4f* projectionMatrix);

};
#endif
