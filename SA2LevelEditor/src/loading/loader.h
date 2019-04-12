#ifndef LOADER_H
#define LOADER_H

class TexturedModel;

#include <unordered_map>
#include <glad/glad.h>
#include <list>
#include <vector>
#include "../models/rawmodel.h"


struct TextureEntry
{
	GLuint id;
	int count;
};

class Loader
{
private:
	static std::list<GLuint> vaos;
	static std::list<GLuint> vbos;

	static std::unordered_map<std::string, TextureEntry> textures;
	static std::unordered_map<GLuint, std::string> texIdToFilename;

	static int vaoNumber;
	static int vboNumber;

	static GLuint createVAO();

	static GLuint storeDataInAttributeList(int, int, std::vector<float>*);

	static void unbindVAO();

	static GLuint bindIndiciesBuffer(std::vector<int>*);

public:
	//For 3D Models
	static RawModel loadToVAO(std::vector<float>* positions, 
							  std::vector<float>* textureCoords, 
							  std::vector<float>* normals, 
							  std::vector<float>* vertexColors, 
							  std::vector<int>* indices);

    //for gui
	static RawModel loadToVAO(std::vector<float>* positions, int dimensions);

	//Loads a texture into GPU memory, returns the GLuint id
	static GLuint loadTexture(const char* filename);

	//Loads a texture without any interpolation
	static GLuint loadTextureNoInterpolation(const char* fileName);

	static void cleanUp();

	static void deleteVAO(GLuint vaoID);

	static void deleteVBO(GLuint vboID);

	static void deleteTexture(GLuint texID);

	static void deleteTexturedModels(std::list<TexturedModel*>* tm);

	static void printInfo();

	static GLuint loadShader(const char* file, int shaderType);
};

#endif
