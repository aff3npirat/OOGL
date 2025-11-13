#pragma once


template<typename T> inline void Mesh::addVertexData(const BufferView* view, const T* data)
{
    this->data.emplace_back(view, static_cast<const void*>(data));
    buffers.push_back(view->buffer);
}
