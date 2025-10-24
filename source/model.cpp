#include "model.h"


ModelBase::Batch::Batch(unsigned int offset, unsigned int numVertex)
{
    this->offset = offset;
    this->numVertex = numVertex;
}


bool Textured3D::compare(const Textured3D* a, const Textured3D* b)
{
    return a->texture < b->texture;
}


Textured3D::Textured3D(BufferView* vertexBuffer, GLfloat* vertices, BufferView* uvBuffer,
    GLfloat* uvs, GLuint texture, unsigned int numVertex)
{
    this->vertices = vertices;
    this->vertexBuffer = vertexBuffer;
    this->uvs = uvs;
    this->uvBuffer = uvBuffer;
    this->texture = texture;
    this->numVertex = numVertex;
}


void Textured3D::insert(unsigned int offset) const
{
    vertexBuffer->insert(vertices, numVertex * 3, offset * 3);
    uvBuffer->insert(uvs, numVertex * 2, offset * 2);
}


unsigned int Textured3D::getNumVertex() const
{
    return numVertex;
}


GLuint Textured3D::getTexture() const
{
    return texture;
}


void Textured3D::Batch::enter() const
{
    glBindTexture(GL_TEXTURE_2D, texture);
}


void Textured3D::Batch::exit() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}


void Textured3D::Batch::initialize(const Textured3D* model)
{
    texture = model->getTexture();
}