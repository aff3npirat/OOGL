#pragma once

#include <GL/Glew.h>

#include <cstddef>
#include <cstdint>

#include <utility>


class VertexBuffer {
  public:
    VertexBuffer(std::size_t size);
    ~VertexBuffer();

    /// @brief Inserts data into buffer.
    /// @param values Data to copy.
    /// @param n Number of bytes to copy from `values`.
    /// @param vertexSize Number of bytes per vertex.
    /// @param stride Number of bytes between data of two consecutive vertices.
    /// @param offset Byte offset from buffer begin.
    void add(const void* values, std::size_t n, std::size_t vertexSize,
        std::size_t stride, std::size_t offset);

    /// @brief Copies content to GPU.
    void use(GLenum mode);

    GLuint id() const { return _id; }
    std::size_t size() const { return _size; }
  
  protected:
    void resize(std::size_t size);

    std::uint8_t* data;
    std::size_t _size;
    GLuint _id;
};