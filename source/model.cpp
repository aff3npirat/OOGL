#include "model.h"


ModelBase::Batch::Batch(unsigned int offset, unsigned int numVertex)
{
    this->offset = offset;
    this->numVertex = numVertex;
}


bool Textured3D::compare(Textured3D a, Textured3D b)
{
    return a.texture < b.texture;
}


Textured3D::Textured3D(BufferView *vertexBuffer, GLfloat *vertices, BufferView *uvBuffer, GLfloat *uvs, GLuint texture, unsigned int numVertex)
{
    this->vertices = vertices;
    this->vertexBuffer = vertexBuffer;
    this->uvs = uvs;
    this->uvBuffer = uvBuffer;
    this->texture = texture;
    this->numVertex = numVertex;
}


void Textured3D::insert(unsigned int offset)
{
    vertexBuffer->insert(vertices, numVertex*3, offset*3);
    uvBuffer->insert(uvs, numVertex*2, offset*2);
}


unsigned int Textured3D::getNumVertex()
{
    return numVertex;
}


GLuint Textured3D::getTexture()
{
    return texture;
}


void Textured3D::Batch::enter()
{
    glBindTexture(GL_TEXTURE_2D, texture);
}


void Textured3D::Batch::exit()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}


void Textured3D::Batch::initialize(Textured3D *model)
{
    texture = model->getTexture();
}