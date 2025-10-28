#pragma once

#include "buffer.h"


class Mesh {
  public:
    Mesh(const BufferView** buffers, const VData** attribs, unsigned int size,
        unsigned int numVertex);

    void insert(unsigned int offset) const;
    unsigned int getNumVertex() const;

  private:
    unsigned int numVertex;
    unsigned int numAttribs;
    const BufferView** buffers;
    const VData** attribs;
};


class TexturedMesh : public Mesh {
  public:
    TexturedMesh(const BufferView** buffers, const VData** attribs, unsigned int size,
        unsigned int numVertex, GLuint texture);

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