#include "buffer.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cassert>


VertexBuffer::VertexBuffer(std::size_t size) : _size(size)
{
    glGenBuffers(1, &_id);
    data = new std::uint8_t[_size];
}


VertexBuffer::~VertexBuffer()
{
    delete[] data;
}


void VertexBuffer::add(const void* values, std::size_t n, std::size_t vertexSize,
    std::size_t stride, std::size_t offset)
{
    // TODO choose better size
    std::size_t requiredSize = (n / vertexSize) * stride + offset;
    if (requiredSize > _size) {
        resize(requiredSize);
    }

    assert((n / vertexSize) * vertexSize == n);
    assert((n / vertexSize) * stride <= _size);

    if (stride == 0) {
            std::memcpy(data + offset, values, n);
            return;
    }

    const std::uint8_t* src = static_cast<const std::uint8_t*>(values);
    std::uint8_t* dest = data + offset;
    unsigned int numVertex = n / vertexSize;

    for (std::size_t i = 0; i < numVertex; i++) {
        std::memcpy(
            dest + i * stride,
            src, vertexSize
        );

        src += vertexSize;
    }
}


void VertexBuffer::use(GLenum mode)
{
    glBindBuffer(GL_ARRAY_BUFFER, _id);
    glBufferData(GL_ARRAY_BUFFER, _size, static_cast<void*>(data), mode);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void VertexBuffer::resize(std::size_t size)
{
    std::uint8_t* old = data;
    data = new std::uint8_t[size];
    std::memcpy(data, old, this->_size);
    this->_size = size;

    delete[] old;
}