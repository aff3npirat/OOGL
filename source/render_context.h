#pragma once

#include <GL/Glew.h>

#include <algorithm>
#include <map>
#include <utility>
#include <vector>

#include "buffer.h"
#include "model.h"


/** Manages VAO initialization and data transfer to GPU. */
template<class C>
class Renderer {
  public:
    /**
     * @param bufferViews each view is linked to specific shader attribute.
     * @param size number of @p bufferViews /vertex attributes.
     */
    Renderer(const BufferView* bufferViews, unsigned int size);
    ~Renderer() { delete[] buffers; }
    Renderer(const Renderer& other) = delete;
    Renderer& operator=(const Renderer& other) = delete;

    /** Stores model reference to render.
     *
     * This Method does not render any models and also does
     * not store any data. All changes to @p model will be rendererd
     * when @ref RenderContext#render is called.
     */
    void addModel(const C* model);
    /** Renders all stored models.
     *
     * Data from all stored models will be transferred to GPU and rendered
     * in specific order to minimize glDraw calls.
     */
    void render();

  protected:
    std::vector<const C*> models;
    GLuint vao;
    Buffer** buffers;
    unsigned int numBuffers;
    unsigned int numAttribs;
};


template<> void Renderer<Mesh>::render();
template<> void Renderer<TexturedMesh>::render();

#include "render_context.tpp"