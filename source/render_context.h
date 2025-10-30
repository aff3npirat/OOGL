#pragma once

#include <GL/Glew.h>

#include <algorithm>
#include <map>
#include <utility>
#include <vector>
#include <initializer_list>

#include "buffer.h"
#include "model.h"


template<class M> concept IsMesh = std::is_base_of<Mesh, M>::value;

/// @brief Manages VAO initialization and vertex attribute data transfer to OGL Buffer Objects.
/// @tparam M mesh to use. For Example use @ref TexturedMesh to render meshes with texture.
template<IsMesh M> class Renderer {
  public:
    /// @param bufferViews specifies indices where vertex attribute values are. Element at index i
    /// corresponds to Vertexshader attribute with index @code i @endcode .
    /// @param size number of @p bufferViews .
    Renderer(const BufferView* bufferViews, unsigned int size);
    Renderer(std::initializer_list<BufferView> bufferViews);
    ~Renderer() { delete[] buffers; }
    Renderer(const Renderer& other) = delete;
    Renderer<M>& operator=(const Renderer& other) = delete;

    /// @brief Adds a @ref Mesh to be rendered.
    /// This Method does not modifies any data and also does. All changes to @p mesh will be
    /// rendererd when @ref RenderContext#render is called.
    void addModel(M& mesh);
    /// @brief Renders all stored meshes.
    /// Data from all stored meshes will be transferred to OGL Buffer Objects and rendered.
    void render();

  protected:
    std::vector<M> toRender;
    GLuint vao;
    Buffer** buffers;
    unsigned int numBuffers;
    unsigned int numAttribs;
};


/// @brief renders all meshes with single glDraw call.
template<> void Renderer<Mesh>::render();
/// @brief renders all meshes and minimizes glDraw calls.
template<> void Renderer<TexturedMesh>::render();

#include "render_context.tpp"