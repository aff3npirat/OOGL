#pragma once

#include <algorithm>
#include <map>
#include <utility>
#include <vector>

#include <GL/Glew.h>

#include "buffer.h"
#include "model.h"


template<class C>
concept IsModel = std::is_base_of<ModelBase, C>::value;

template<IsModel Model>
/** Manages VAO initialization and data transfer to GPU. */
class RenderContext {
  public:
    /**
     * @param bufferViews each view is linked to specific shader attribute.
     * @param shaderAttribs shader attributes linked to @p bufferViews .
     * @param size number of @p bufferViews and @p shaderAttribs . 
     */
    RenderContext(BufferView* bufferViews, unsigned int* shaderAttribs, unsigned int size) {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        
        std::map<GLuint, Buffer*> buffers;
        for (int i = 0; i < size; i++) {
            this->shaderAttribs[i] = shaderAttribs[i];

            glBindBuffer(GL_ARRAY_BUFFER, bufferViews[i].buffer->uid());
            glVertexAttribPointer(
                shaderAttribs[i],
                bufferViews[i].vertexSize,
                bufferViews[i].glType,
                GL_FALSE,
                bufferViews[i].stride * bufferViews[i].buffer->byteSize(),
                bufferViews[i].offset * bufferViews[i].buffer->byteSize()
            );

            if (!buffers.contains(bufferViews[i].buffer->uid())) {
                buffers[bufferViews[i].buffer->uid()] = bufferViews[i].buffer;
            }
        }
        numAttribs = size;

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        numBuffers = buffers.size();
        this->buffers = new Buffer*[numBuffers];
        Buffer** it = buffers.begin();
        for (int i = 0; i < numBuffers; i++) {
            this->buffers[i] = it[i];
        }
    }
    ~RenderContext() { delete[] buffers; }

    /** Stores model reference to render.
     * 
     * This Method does not render any models and also does
     * not store any data. All changes to @p model will be rendererd
     * when @ref RenderContext#render is called.
     */
    void addModel(Model& model) { models.push_back(model); }
    void render() {
        std::sort(models.begin(), models.end(), Model::compare);

        unsigned int offset = 0;
        std::vector<typename Model::Batch> batches;
        batches.emplace_back(offset, 0);
        batches.back().initialize(models[0]);

        for (int i = 1; i < models.size(); i++) {
            if (Model::compare(models[i-1], models[i])) {
                batches.emplace_back(offset, 0);
                batches.back().initialize(models[i]);
            }

            models[i].insert(offset);

            offset += models[i].getNumVertex();
            batches.back().numVertex += models[i].getNumVertex();
        }

        for (int i = 0; i < numBuffers; i++) {
            glBindBuffer(GL_ARRAY_BUFFER, buffers[i]->uid());
            glBufferData(GL_ARRAY_BUFFER, buffers[i]->byteSize() * buffers[i]->size(), buffers[i]->data(), GL_STATIC_DRAW);
        }

        for (int i = 0; i < numAttribs; i++) {
            glEnableVertexAttribArray(shaderAttribs[i]);
        }
        for (int i = 0; i < batches.size(); i++) {
            batches[i].enter();
            glDrawArrays(GL_TRIANGLES, batches[i].offset, batches[i].numVertex);
            batches[i].exit();
        }
        for (int i = 0; i < numAttribs; i++) {
            glDisableVertexAttribArray(shaderAttribs[i]);
        }
    }

  private:
    GLuint vao;
    Buffer** buffers;
    unsigned int numBuffers;
    unsigned int* shaderAttribs;
    unsigned int numAttribs;
    std::vector<Model*> models;
};
