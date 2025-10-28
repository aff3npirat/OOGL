#pragma once

#include <cstdint>


template<IsMesh M> void Renderer<M>::addModel(const M* mesh)
{
    toRender.push_back(mesh);
}


template<IsMesh M> Renderer<M>::Renderer(const BufferView* bufferViews, unsigned int size)
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    numAttribs = size;

    std::map<GLuint, Buffer*> buffers;
    for (int i = 0; i < size; i++) {
        glBindBuffer(GL_ARRAY_BUFFER, bufferViews[i].buffer->id());
        glVertexAttribPointer(i, bufferViews[i].vertexSize, bufferViews[i].glType, GL_FALSE,
            bufferViews[i].stride * bufferViews[i].buffer->byteSize(),
            (void*)(std::intptr_t)(bufferViews[i].offset * bufferViews[i].buffer->byteSize()));

        if (!buffers.contains(bufferViews[i].buffer->id())) {
            buffers[bufferViews[i].buffer->id()] = bufferViews[i].buffer;
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    numBuffers = buffers.size();
    this->buffers = new Buffer*[numBuffers];
    std::map<GLuint, Buffer*>::iterator it = buffers.begin();
    for (int i = 0; i < numBuffers; i++) {
        this->buffers[i] = it->second;
        it++;
    }
}