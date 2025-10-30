#include "model.h"

#include <algorithm>


VData::~VData()
{
    delete ptr;
}


VData::VData()
{
    this->ptr = nullptr;
    this->attribSize = 0;
}


VData::VData(const VData& other) : attribSize(other.attribSize), ptr(other.ptr->createCopy()) {}


VData::VData(VData&& other) : VData()
{
    swap(*this, other);
}


VData& VData::operator=(VData other)
{
    swap(*this, other);
    return *this;
}


void VData::insert(const BufferView* buffer, unsigned int offset) const
{
    ptr->insert(buffer, offset * attribSize);
}


Mesh::Mesh(
    const BufferView** buffers, const VData* attribs, unsigned int size, unsigned int numVertex)
{
    this->buffers = new const BufferView*[size];
    this->attribs = new VData[size];
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


Mesh::Mesh(const Mesh& other) : numVertex(other.numVertex), numAttribs(other.numAttribs)
{
    this->buffers = new const BufferView*[numAttribs];
    this->attribs = new VData[numAttribs];
    for (int i = 0; i < numAttribs; i++) {
        this->buffers[i] = other.buffers[i];
        this->attribs[i] = other.attribs[i];
    }
}


Mesh::Mesh(Mesh&& other) : Mesh()
{
    swap(*this, other);
}


Mesh& Mesh::operator=(Mesh other)
{
    swap(*this, other);
    return *this;
}


void Mesh::insert(unsigned int offset) const
{
    for (int i = 0; i < numAttribs; i++) {
        attribs[i].insert(buffers[i], offset);
    }
}


unsigned int Mesh::getNumVertex() const
{
    return numVertex;
}

Mesh::Mesh()
{
    this->attribs = nullptr;
    this->buffers = nullptr;
    this->numAttribs = 0;
    this->numVertex = 0;
}


TexturedMesh::TexturedMesh(const BufferView** buffers, const VData* attribs, unsigned int size,
    unsigned int numVertex, GLuint texture)
    : Mesh(buffers, attribs, size, numVertex)
{
    this->texture = texture;
}


TexturedMesh::TexturedMesh(const TexturedMesh& other) : Mesh(other)
{
    this->texture = other.texture;
}


TexturedMesh::TexturedMesh(TexturedMesh&& other) : TexturedMesh()
{
    swap(*this, other);
}

TexturedMesh& TexturedMesh::operator=(TexturedMesh other)
{
    swap(*this, other);
    return *this;
}


GLuint TexturedMesh::getTexture() const
{
    return texture;
}


TexturedMesh::TexturedMesh() : Mesh()
{
    this->texture = 0;
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
}


Textured3DModel::~Textured3DModel()
{
    delete[] vertices;
    delete[] uvs;
}


TexturedMesh Textured3DModel::getMesh(
    const BufferView* vertexBuffer, const BufferView* uvBuffer) const
{
    const BufferView* buffers[2] = {vertexBuffer, uvBuffer};
    VData attribs[2] = {VData(vertices, numVertex * 3, 3), VData(uvs, numVertex * 2, 2)};
    return TexturedMesh(buffers, attribs, 2, numVertex, texture);
}


void swap(VData& a, VData& b)
{
    using std::swap;
    swap(a.ptr, b.ptr);
    swap(a.attribSize, b.attribSize);
}


void swap(Mesh& a, Mesh& b)
{
    using std::swap;
    swap(a.numAttribs, b.numAttribs);
    swap(a.numVertex, b.numVertex);
    swap(a.attribs, b.attribs);
    swap(a.buffers, b.buffers);
}


void swap(TexturedMesh& a, TexturedMesh& b)
{
    using std::swap;
    // TODO call base class swap
    swap(static_cast<Mesh&>(a), static_cast<Mesh&>(b));
    swap(a.texture, b.texture);
}
