#pragma once
#include "buffer.h"


template<typename T> inline Buffer::Implement<T>::Implement(unsigned int size)
{
    values = new T[size];
    this->size = size;
    byteSize = sizeof(T);

    glGenBuffers(1, &id);
}


template<typename T> inline Buffer::Implement<T>::~Implement()
{
    delete[] values;
}


template<typename T> inline void* Buffer::Implement<T>::getValues() const
{
    return (void*)values;
}


template<typename T> inline void Buffer::init(std::in_place_type_t<T>, unsigned int size)
{
    ptr = new Implement<T>(size);
}


template<typename T>
inline void BufferView::insert(const T* values, unsigned int size, unsigned int tempOffset) const
{
    T* first = static_cast<T*>(buffer->data()) + offset + tempOffset * stride;

    int idx = 0;
    int stridedIdx = 0;
    while (idx < size) {
        for (int j = 0; j < vertexSize; j++) {
            first[stridedIdx + j] = values[idx + j];
        }

        idx += vertexSize;
        stridedIdx += stride;
    }
}