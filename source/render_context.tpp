#pragma once

#include <cstdint>


template<IsModel Model>
inline void Render<Model>::addModel(const Model* model)
{
    models.push_back(model);
}


template<IsModel Model>
inline Render<Model>::Render(const BufferView* bufferViews, unsigned int size)
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


template<IsModel Model>
inline void Render<Model>::render()
{
    std::sort(models.begin(), models.end(), Model::compare);

    unsigned int offset = 0;
    std::vector<typename Model::Batch> batches;
    batches.emplace_back(offset, models[0]->getNumVertex());
    batches.back().initialize(models[0]);
    models[0]->insert(offset);

    for (int i = 1; i < models.size(); i++) {
        if (Model::compare(models[i - 1], models[i])) {
            batches.emplace_back(offset, 0);
            batches.back().initialize(models[i]);
        }

        models[i]->insert(offset);

        offset += models[i]->getNumVertex();
        batches.back().numVertex += models[i]->getNumVertex();
    }

    glBindVertexArray(vao);
    for (int i = 0; i < numBuffers; i++) {
        glBindBuffer(GL_ARRAY_BUFFER, buffers[i]->id());
        glBufferData(GL_ARRAY_BUFFER, buffers[i]->byteSize() * buffers[i]->size(),
            buffers[i]->data(), GL_STATIC_DRAW);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    for (GLint i = 0; i < numAttribs; i++) {
        glEnableVertexAttribArray(i);
    }
    for (int i = 0; i < batches.size(); i++) {
        batches[i].enter();
        glDrawArrays(GL_TRIANGLES, batches[i].offset, batches[i].numVertex);
        batches[i].exit();
    }
    for (GLint i = 0; i < numAttribs; i++) {
        glDisableVertexAttribArray(i);
    }
    glBindVertexArray(0);
}