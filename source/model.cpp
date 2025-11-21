#include "model.h"

#include <cstring>
#include <cstdint>


Mesh::Mesh(unsigned int numVertex) : numVertex(numVertex) {}


Mesh::Mesh(Mesh&& other) : numVertex(other.numVertex)
{
    this->buffers = other.buffers;
    this->data = other.data;
}

Mesh& Mesh::operator=(Mesh&& other)
{
    this->numVertex = other.numVertex;
    this->data = other.data;
    this->buffers = other.buffers;
    return *this;
}


Mesh::~Mesh()
{
    for (int i = 0; i < data.size(); i++) {
        delete[] static_cast<const uint8_t*>(data[i].data);
    }
}


void Mesh::insert(unsigned int offset)
{
    for (int i = 0; i < data.size(); i++) {
        VertexData* currVData = &(data[i]);
        
        unsigned int byteOffset = offset * currVData->byteSize + currVData->offset;
        uint8_t* dest = static_cast<uint8_t*>(buffers[i]->data()) + byteOffset;
        const uint8_t* src = static_cast<const uint8_t*>(currVData->data);

        for (int j = 0; j < numVertex; j++) {
            std::memcpy(static_cast<void*>(dest), static_cast<const void*>(src), currVData->vertexSize);

            dest += currVData->stride;
            src += currVData->vertexSize;
        }
    }
}


TexturedMesh::TexturedMesh(unsigned int numVertex, GLuint texture)
    : Mesh(numVertex), texture(texture)
{
}


TexturedMesh::TexturedMesh(TexturedMesh&& other) : Mesh(std::move(other)), texture(other.texture) {}


TexturedMesh& TexturedMesh::operator=(TexturedMesh&& other)
{
    Mesh::operator=(std::move(other));
    this->texture = other.texture;
    return *this;
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
    TexturedMesh mesh(numVertex, texture);
    mesh.addVertexData(vertexBuffer, vertices);
    mesh.addVertexData(uvBuffer, uvs);

    return mesh;
}


VertexData::VertexData(const BufferView* stridedView, const void* data) : data(data)
{
    byteSize = stridedView->buffer->byteSize();
    stride = stridedView->stride * byteSize;
    offset = stridedView->offset * byteSize;
    vertexSize = stridedView->vertexSize * byteSize;
}