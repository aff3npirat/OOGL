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
    Render(const BufferView* bufferViews, unsigned int size);
    ~Render() { delete[] buffers; }
    Render(const Render& other) = delete;
    Render& operator=(const Render& other) = delete;

    /** Stores model reference to render.
     *
     * This Method does not render any models and also does
     * not store any data. All changes to @p model will be rendererd
     * when @ref RenderContext#render is called.
     */
    void addModel(const Model* model);
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
    std::vector<const Model*> models;
};


#include "render_context.tpp"