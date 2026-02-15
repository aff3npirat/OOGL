#include "render_context.h"

#include <GL/Glew.h>

#include <cstddef>
#include <iterator>

#include "buffer.h"
#include "utility.h"


VAO::VAO(GLenum renderMode) : renderMode(renderMode)
{
    glGenVertexArrays(1, &id);
}


VAO::~VAO()
{
    delete[] buffers;

    for (AttributeBinding* binding : attribBindings) {
        delete binding;
    }
}


const AttributeBinding* VAO::bindBuffer(
    const VertexAttribute* attribute, unsigned int index, VertexBuffer* buffer, std::size_t valSize)
{
    AttributeBinding* binding = new AttributeBinding;
    binding->attribute = attribute;
    binding->buffer = buffer;
    binding->index = index;
    binding->valSize = valSize;
    binding->offset = 0;
    binding->stride = -1;  // Set when all attributes for buffer are bound

    std::vector<AttributeBinding*>::iterator it = sortedInsert(attribBindings, binding,
        [](AttributeBinding* a, AttributeBinding* b) { return a->buffer == b->buffer; }
    );

    if (++it != attribBindings.end()) {  // Other binding for buffer exists
        binding->offset = (*it)->offset + (*it)->valSize * (*it)->attribute->size;
    }
    else {
        numBuffers++;
    }

    return binding;
}


void VAO::initialize()
{
    buffers = new VertexBuffer*[numBuffers];
    
    glBindVertexArray(id);
    
    std::size_t bufferIdx = 0;
    std::size_t stride;
    VertexBuffer* prevBuffer = nullptr;
    for (AttributeBinding* binding : attribBindings) {
        if (binding->buffer != prevBuffer) {
            stride = binding->offset + binding->attribute->size * binding->valSize;
            
            prevBuffer = binding->buffer;
            buffers[bufferIdx++] = prevBuffer;
        }

        binding->stride = stride;

        glBindBuffer(GL_ARRAY_BUFFER, binding->buffer->id());
        glVertexAttribPointer(
            binding->index,
            binding->attribute->size,
            binding->attribute->glType,
            binding->attribute->normalized,
            binding->stride,
            (void*)binding->offset
        );
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void VAO::begin()
{    
}


void VAO::end()
{
    for (std::size_t i = 0; i < numBuffers; i++) {
        buffers[i]->use(renderMode);
    }

    numVertex = attribBindings[0]->buffer->size() / attribBindings[0]->stride;
}


void VAO::addData(const AttributeBinding* binding, const void* data, unsigned int numVertex,
    unsigned int vertexOffset)
{
    std::size_t vertexSize = binding->valSize * binding->attribute->size;
    std::size_t offset = binding->offset + vertexOffset * vertexSize;
    binding->buffer->add(
        data, numVertex * vertexSize, vertexSize, binding->stride, binding->offset
    );
}


void VAO::render(unsigned int offset, unsigned int numVertex)
{
    glBindVertexArray(id);
    for (AttributeBinding* binding : attribBindings) {
        glEnableVertexAttribArray((GLint)(binding->index));
    }
    glDrawArrays(GL_TRIANGLES, offset, numVertex);
    for (AttributeBinding* binding : attribBindings) {
        glDisableVertexAttribArray((GLint)(binding->index));
    }
    glBindVertexArray(0);
}