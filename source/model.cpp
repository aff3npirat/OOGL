#include "model.h"


Mesh::Mesh(
    const BufferView** buffers, const VData** attribs, unsigned int size, unsigned int numVertex)
{
    this->buffers = buffers;
    this->attribs = attribs;
    this->numVertex = numVertex;
    this->numAttribs = size;
}


void Mesh::insert(unsigned int offset) const
{
    for (int i = 0; i < numAttribs; i++) {
        attribs[i]->insert(buffers[i], offset * attribs[i]->getAttribSize());
    }
}


unsigned int Mesh::getNumVertex() const
{
    return numVertex;
}


TexturedMesh::TexturedMesh(const BufferView** buffers, const VData** attribs, unsigned int size,
    unsigned int numVertex, GLuint texture)
    : Mesh(buffers, attribs, size, numVertex)
{
    this->texture = texture;
}


GLuint TexturedMesh::getTexture() const
{
    return texture;
}


Textured3DModel::Textured3DModel(
    GLfloat* vertices, GLfloat* uvs, unsigned int numVertex, GLuint texture)
{
    this->vertices = vertices;
    this->uvs = uvs;
    this->numVertex = numVertex;
    this->texture = texture;
}


TexturedMesh Textured3DModel::getMesh(
    const BufferView* vertexBuffer, const BufferView* uvBuffer) const
{
    // TODO fix memory issues
    const BufferView** buffers = new const BufferView*[2];
    buffers[0] = vertexBuffer;
    buffers[1] = uvBuffer;

    const VData** attribs = new const VData*[2];
    attribs[0] = new VData(vertices, numVertex * 3, 3);
    attribs[1] = new VData(uvs, numVertex * 2, 2);

    return TexturedMesh(buffers, attribs, 2, numVertex, texture);
}