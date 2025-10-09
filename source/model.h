#pragma once

#include "render_context.h"


class ModelBase {
  public:
    struct Batch {
        Batch(unsigned int offset, unsigned int numVertex);

        void initialize(ModelBase* model) {}
        void enter() {}
        void exit() {}
        
        unsigned int offset;
        unsigned int numVertex;
    };

    virtual ~ModelBase() = 0;
    virtual void insert(unsigned int offset) = 0;
    virtual unsigned int getNumVertex() = 0;
    static bool compare(ModelBase a, ModelBase b);
};


class Textured3D : public ModelBase {
  public:
    struct Batch : public ModelBase::Batch {
        void enter();
        void exit();
        void initialize(Textured3D* model);
        
        GLuint texture;
    };

    static bool compare(Textured3D a, Textured3D b);

    Textured3D(BufferView* vertexBuffer, GLfloat* vertices, BufferView* uvBuffer, GLfloat* uvs, GLuint texture, unsigned int numVertex);

    void insert(unsigned int offset);
    unsigned int getNumVertex();
    GLuint getTexture();

  private:
    GLfloat* vertices;
    BufferView* vertexBuffer;
    GLfloat* uvs;
    BufferView* uvBuffer;
    GLuint texture;
    unsigned int numVertex;
};