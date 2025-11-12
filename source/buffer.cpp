#include "buffer.h"


Buffer::~Buffer()
{
    delete ptr;
}


unsigned int Buffer::byteSize() const
{
    return ptr->byteSize;
}


unsigned int Buffer::size() const
{
    return ptr->size;
}


void* Buffer::data() const
{
    return ptr->getValues();
}


unsigned int Buffer::id() const
{
    return ptr->id;
}