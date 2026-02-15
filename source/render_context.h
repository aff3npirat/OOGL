#pragma once

#include <GL/Glew.h>

#include <cstddef>
#include <vector>

#include "buffer.h"


struct VertexAttribute {
    unsigned int size;
    GLenum glType;
    GLboolean normalized;
};


struct AttributeBinding {
    const VertexAttribute* attribute;
    VertexBuffer* buffer;
    unsigned int index;
    // Layout of vertex data (in bytes)
    std::size_t offset;
    std::size_t stride;
    std::size_t valSize;
};


class VAO {
  public:
    VAO(GLenum renderMode);
    ~VAO();

    /// @brief Creates binding between a vertex attribute and a vertex buffer.
    /// @param attribute Specifies vertex attribute.
    /// @param index Index of vertex attribute in shader.
    /// @param buffer Where vertex data is stored (before send to GPU memory).
    /// @param valSize Size in bytes of single vertex attribute value.
    /// return Binding reference.
    const AttributeBinding* bindBuffer(const VertexAttribute* attribute, unsigned int index,
        VertexBuffer* buffer, std::size_t valSize);

    /// @brief Initializes VAO by specifying attribute data layouts.
    /// Should be called after all attributes are bound to VAO.
    void initialize();

    /// @brief Initializes data collection.
    void begin();

    /// @brief Signals that no more data will be added.
    void end();

    /// @brief Copies vertex data into buffer.
    /// @param binding Buffer binding to insert values into.
    /// @param data Values to insert into buffer.
    /// @param numVertex Number of vertices to insert.
    void addData(const AttributeBinding* binding, const void* data, unsigned int numVertex,
        unsigned int vertexOffset = 0);

    /// @brief Renders all added vertices.
    /// @param offset Index of first vertex to draw.
    /// @param numVertex Number of vertices to draw.
    void render(unsigned int offset, unsigned int numVertex);

    unsigned int getNumVertex() { return numVertex; }

  private:
    GLuint id;
    GLenum renderMode;
    std::vector<AttributeBinding*> attribBindings;
    std::size_t numBuffers = 0;
    VertexBuffer** buffers;
    unsigned int numVertex = 0;
};