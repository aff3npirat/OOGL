#include "buffer.h"


BufferView::BufferView(
    Buffer* buffer, unsigned int stride, unsigned int offset, unsigned int vertexSize, int glType)
{
    this->buffer = buffer;
    this->stride = (stride == 0) ? vertexSize : stride;
    this->offset = offset;
    this->vertexSize = vertexSize;
    this->glType = glType;
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
