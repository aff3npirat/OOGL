#pragma once
#include "buffer.h"

#include <utility>


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


template<typename T> inline void Buffer::init(std::in_place_type_t<T>, unsigned int size)
{
    ptr = new Implement<T>(size);
}