#include "buffer.h"


Buffer::Buffer() : ptr(nullptr), size(0), byteSize(0)
{
    glGenBuffers(1, &id);
}


Buffer::~Buffer()
{
    glDeleteBuffers(1, &id);
    delete ptr;
}