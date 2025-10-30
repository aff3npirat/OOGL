#include "model.h"
#pragma once

template<typename T> inline VData::VData(const T* values, unsigned int n, unsigned int size)
{
    ptr = new Implement<T>(values, n);
    attribSize = size;
}


template<typename T> inline VData::Implement<T>::Implement(const T* values, unsigned int size)
{
    this->values = values;
    this->size = size;
}


template<typename T>
inline void VData::Implement<T>::insert(const BufferView* buffer, unsigned int offset) const
{
    buffer->insert(values, size, offset);
}


template<typename T> inline VData::Base* VData::Implement<T>::createCopy() const
{
    Implement<T>* copy = new Implement<T>(this->values, this->size);
    return copy;
}
