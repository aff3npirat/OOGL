#pragma once

#include "buffer.h"


/// @brief Implements Polymorphic type wrapping Vertex Attribute data.
/// Stores a pointer to vertex attribute values of any type aswell as utility information needed for
/// correct transfer to OLG Buffer Object.
class VData {
  public:
    /// @tparam T type of wrapped values.
    /// @param values pointer to wrapped values.
    /// @param n number of values to wrap (i.e. array size).
    /// @param size number of values corresponding to single vertex (e.g. commonly 2 for
    /// UV-Coordinates, 3 for 3D Vertex positions). @p values point to array of size @code n * size
    /// @endcode .
    template<typename T> VData(const T* values, unsigned int n, unsigned int size);
    ~VData();

    /// @brief Inserts values into referenced buffers.
    /// @param buffer specifies indices to insert values at.
    /// @param offset offset in vertices for first value.
    void insert(const BufferView* buffer, unsigned int offset) const;

  private:
    struct Base {
        ~Base() {};
        virtual void insert(const BufferView* buffer, unsigned int offset) const = 0;
        unsigned int size;
    };

    template<typename T> struct Implement : public Base {
        Implement(const T* values, unsigned int size);
        ~Implement();

        void insert(const BufferView* buffer, unsigned int offset) const;

        const T* values;
    };

    Base* ptr;
    unsigned int attribSize;
};


/// @brief Collection of Vertex Attributes assigned to specific buffers.
class Mesh {
  public:
    /// @param buffers indices at which to insert each vertex attribute.
    /// @param attribs values of each vertex attribute.
    /// @param size number of attributes/buffers.
    /// @param numVertex number of vertices. Each vertex attribute should have values for @p
    /// numVertex vertices.
    Mesh(const BufferView** buffers, const VData** attribs, unsigned int size,
        unsigned int numVertex);
    /// @brief Inserts each vertex attribute into corresponding @ref Buffer .
    /// @param offset offset of first value for each vertex attribute. Offset is specified in
    /// vertices not array values.
    void insert(unsigned int offset) const;
    
    unsigned int getNumVertex() const;

  private:
    unsigned int numVertex;
    unsigned int numAttribs;
    const BufferView** buffers;
    const VData** attribs;
};


/// @brief Collection of multiple Vertex Attributes and a texture.
class TexturedMesh : public Mesh {
  public:
    /// @copydoc Mesh#Mesh 
    /// @param texture texture-id.
    TexturedMesh(const BufferView** buffers, const VData** attribs, unsigned int size,
        unsigned int numVertex, GLuint texture);

    /// returns unique OGL texture-id.
    GLuint getTexture() const;

  private:
    GLuint texture;
};


class ModelBase {
  public:
    virtual ~ModelBase() {}

    virtual void transpose() = 0;
    virtual void rotate() = 0;
    virtual void scale() = 0;
};


class Textured3DModel : public ModelBase {
  public:
    Textured3DModel(GLfloat* vertices, GLfloat* uvs, unsigned int numVertex, GLuint texture);

    void transpose() {};
    void rotate() {};
    void scale() {};
    TexturedMesh getMesh(const BufferView* vertexBuffer, const BufferView* uvBuffer) const;

  private:
    GLfloat* vertices;
    GLfloat* uvs;
    GLuint texture;
    unsigned int numVertex;
};


#include "model.tpp"