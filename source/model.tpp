#pragma once


template<typename T> inline void Mesh::addVertexData(const BufferView* view, const T* data)
{
    unsigned int byteSize = sizeof(T);
    this->data.emplace_back(static_cast<const void*>(data), view->stride * byteSize,
        view->offset * byteSize, view->vertexSize, byteSize);
    buffers.push_back(view->buffer);
}
