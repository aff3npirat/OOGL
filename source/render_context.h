#pragma once

#include <GL/Glew.h>

#include <algorithm>
#include <map>
#include <utility>
#include <vector>

#include "buffer.h"
#include "model.h"


template<class C> concept IsModel = std::is_base_of<ModelBase, C>::value;

template<IsModel Model>
/** Manages VAO initialization and data transfer to GPU. */
class Render {
  public:
    /**
     * @param bufferViews each view is linked to specific shader attribute.
     * @param size number of @p bufferViews /vertex attributes.
     */
    Render(BufferView* bufferViews, unsigned int size);
    ~Render() { delete[] buffers; }

    /** Stores model reference to render.
     *
     * This Method does not render any models and also does
     * not store any data. All changes to @p model will be rendererd
     * when @ref RenderContext#render is called.
     */
    void addModel(Model* model) { models.push_back(model); }
    /** Renders all stored models.
     *
     * Data from all stored models will be transferred to GPU and rendered
     * in specific order to minimize glDraw calls.
     */
    void render();

  private:
    GLuint vao;
    Buffer** buffers;
    unsigned int numBuffers;
    unsigned int numAttribs;
    std::vector<Model*> models;
};


template<IsModel Model>
inline Render<Model>::Render(BufferView* bufferViews, unsigned int size)
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    numAttribs = size;

    std::map<GLuint, Buffer*> buffers;
    for (int i = 0; i < size; i++) {
        glBindBuffer(GL_ARRAY_BUFFER, bufferViews[i].buffer->id());
        glVertexAttribPointer(i, bufferViews[i].vertexSize, bufferViews[i].glType, GL_FALSE,
            bufferViews[i].stride * bufferViews[i].buffer->byteSize(),
            (void*)(bufferViews[i].offset * bufferViews[i].buffer->byteSize()));

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