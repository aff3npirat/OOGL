#pragma once

#include <cstring>


template<typename T> inline void Mesh::addVertexData(const BufferView* view, const T* data)
{
    std::size_t size = numVertex * view->vertexSize * sizeof(T);
    void* dataCpy = static_cast<void*>(new uint8_t[size]);
    std::memcpy(dataCpy, static_cast<const void*>(data), size);

    this->data.emplace_back(view, dataCpy);
    buffers.push_back(view->buffer);
}
