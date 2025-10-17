#include "buffer.h"


BufferView::BufferView(
    Buffer* buffer, unsigned int stride, unsigned int offset, unsigned int vertexSize, int glType)
{
    this->buffer = buffer;
    this->stride = stride;
    this->offset = offset;
    this->vertexSize = vertexSize;
    this->glType = glType;
}


unsigned int Buffer::byteSize()
{
    return ptr->byteSize;
}


unsigned int Buffer::size()
{
    return ptr->size;
}


void* Buffer::data()
{
    return ptr->getValues();
}


unsigned int Buffer::id()
{
    return ptr->id;
}
