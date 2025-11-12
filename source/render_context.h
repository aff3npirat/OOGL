#pragma once

#include <GL/Glew.h>

#include <algorithm>
#include <initializer_list>
#include <vector>

#include "buffer.h"
#include "model.h"


struct RenderBatch {
    unsigned int offset;
    unsigned int numVertex;
};

struct TextureBatch {
    unsigned int offset;
    unsigned int numVertex;
    GLuint texture;
};

template<class T> concept IsMesh = std::is_base_of<Mesh, T>::value;
template<class T> concept IsBatch = requires(T a) {
    { a.numVertex } -> std::convertible_to<unsigned int>;
    { a.offset } -> std::convertible_to<unsigned int>;
};

/// @brief Manages VAO initialization and vertex attribute data transfer to OGL Buffer Objects.
/// @tparam M mesh to use. For Example use @ref TexturedMesh to render meshes with texture.
template<IsMesh M, IsBatch B> class Renderer {
  public:
    /// @param bufferViews specifies indices where vertex attribute values are. Element at index i
    /// corresponds to Vertexshader attribute with index @code i @endcode .
    /// @param size number of @p bufferViews .
    Renderer(const BufferView* bufferViews, unsigned int size);
    Renderer(std::initializer_list<BufferView> bufferViews);
    ~Renderer();
    Renderer(const Renderer& other) = delete;
    Renderer<M, B>& operator=(const Renderer<M, B>& other) = delete;

    /// @brief Adds a @ref Mesh to be rendered.
    /// This Method does not modifies any data and also does. All changes to @p mesh will be
    /// rendererd when @ref RenderContext#render is called.
    void addModel(M& mesh);
    /// @brief Renders all stored meshes.
    /// Data from all stored meshes will be transferred to OGL Buffer Objects and rendered.
    void render();
    void begin();
    void end();

  protected:
    void generateBatches();

    std::vector<M> toRender;
    std::vector<B> renderBatches;
    GLuint vao;
    Buffer** buffers;
    unsigned int numBuffers;
    unsigned int numAttribs;
};

/// @brief renders all meshes with single glDraw call.
template<> void Renderer<Mesh, RenderBatch>::render();
template<> void Renderer<Mesh, RenderBatch>::generateBatches();
/// @brief renders all meshes and minimizes glDraw calls.
template<> void Renderer<TexturedMesh, TextureBatch>::render();
template<> void Renderer<TexturedMesh, TextureBatch>::generateBatches();


#include "render_context.tpp"