#pragma once
#include "buffer.h"

#include <utility>


template<typename T> inline Buffer::Implement<T>::Implement(unsigned int size)
{
    values = new T[size];
}


template<typename T> inline Buffer::Implement<T>::~Implement()
{
    delete[] values;
}


template<typename T> inline void Buffer::resize(std::in_place_type_t<T>, unsigned int size)
{
    delete ptr;

    ptr = new Implement<T>(size);
    this->size = size;
    byteSize = sizeof(T);
}