#include "model.h"

#include <algorithm>


VData::~VData()
{
    delete ptr;
}


void VData::insert(const BufferView* buffer, unsigned int offset) const
{
    ptr->insert(buffer, offset * attribSize);
}


Mesh::Mesh(
    const BufferView** buffers, const VData** attribs, unsigned int size, unsigned int numVertex)
{
    this->buffers = new const BufferView*[size];
    this->attribs = new const VData*[size];
    for (int i = 0; i < size; i++) {
        this->buffers[i] = buffers[i];
        this->attribs[i] = attribs[i];
    }

    this->numVertex = numVertex;
    this->numAttribs = size;
}


Mesh::~Mesh()
{
    delete[] buffers;
    delete[] attribs;
}


void Mesh::insert(unsigned int offset) const
{
    for (int i = 0; i < numAttribs; i++) {
        attribs[i]->insert(buffers[i], offset);
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
    this->vertices = new GLfloat[numVertex * 3];
    std::copy(vertices, vertices + numVertex * 3, this->vertices);
    this->uvs = new GLfloat[numVertex * 2];
    std::copy(uvs, uvs + numVertex * 2, this->uvs);

    this->numVertex = numVertex;
    this->texture = texture;
    this->vertexAttrib = new const VData(this->vertices, numVertex * 3, 3);
    this->uvAttrib = new const VData(this->uvs, numVertex * 2, 2);
}


Textured3DModel::~Textured3DModel()
{
    delete[] vertices;
    delete[] uvs;
    delete vertexAttrib;
    delete uvAttrib;
}


TexturedMesh Textured3DModel::getMesh(
    const BufferView* vertexBuffer, const BufferView* uvBuffer) const
{
    const BufferView* buffers[2] = {vertexBuffer, uvBuffer};
    const VData* attribs[2] = {vertexAttrib, uvAttrib};

    return TexturedMesh(buffers, attribs, 2, numVertex, texture);
}