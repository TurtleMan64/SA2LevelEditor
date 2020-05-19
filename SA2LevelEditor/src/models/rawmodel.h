#ifndef RAWMODEL_H
#define RAWMODEL_H

#include <glad/glad.h>
#include <list>

class RawModel
{
private:
    GLuint vaoID;
    int vertexCount;
    std::list<GLuint> vboIDs;

public:
    RawModel();

    RawModel(GLuint vaoID, int vertexCount, std::list<GLuint>* vboIDs);

    GLuint getVaoID();
    void setVaoID(GLuint newID);

    void setVertexCount(int newCount);
    int getVertexCount();

    void deleteMe();

    //for use in textured model constructor only
    std::list<GLuint>* getVboIDs();
};

#endif
