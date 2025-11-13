#pragma once

#include <vector>

#include "buffer.h"


struct VertexData {
    VertexData(const BufferView* stridedView, const void* data);

    const void* data;
    unsigned int stride;
    unsigned int offset;
    unsigned int vertexSize;
    unsigned int byteSize;
};

class Mesh {
  public:
    Mesh(unsigned int numVertex);
    Mesh(Mesh&& other);
    Mesh& operator=(Mesh&& other);

    template<typename T> void addVertexData(const BufferView* view, const T* data);
    void insert(unsigned int byteOffset);

    unsigned int getNumVertex() const { return numVertex; }

  protected:
    Mesh() = default;

  private:
    unsigned int numVertex;
    std::vector<VertexData> data;
    std::vector<Buffer*> buffers;
};

class TexturedMesh : public Mesh {
  public:
    TexturedMesh(unsigned int numVertex, GLuint texture);
    TexturedMesh(TexturedMesh&& other);
    TexturedMesh& operator=(TexturedMesh&& other);

    GLuint getTexture() const { return texture; }

  protected:
    TexturedMesh() = default;

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
    ~Textured3DModel();

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