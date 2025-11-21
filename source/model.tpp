#pragma once

#include <cstring>


template<typename T> inline void Mesh::addVertexData(const BufferView* view, const T* data)
{
    std::size_t size = numVertex * view->vertexSize;
    const void* dataCpy = static_cast<void*>(new T[size]);
    std::memcpy(dataCpy, static_cast<const void*>(data), size * sizeof(T));

    this->data.emplace_back(view, static_cast<const void*>(dataCpy));
    buffers.push_back(view->buffer);
}
