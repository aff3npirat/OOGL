#pragma once

#include "buffer.h"


/** Interface of a collection of Vertex data and sorting logic.
 * 
 * Stores vertex data and provides methods to modify these (e.g. rotating, transposing).
 * Also implements sorting logic to enable optimized rendering.
 */
class ModelBase {
  public:
    /** Context to render multiple models with single glDraw call. */
    struct Batch {
        /**
         * @param offset offset for first vertex to render. Offset is given in vertices not buffer values/bytes.
         * @param numVertex number of vertices to render.
         */
        Batch(unsigned int offset, unsigned int numVertex);

        /**
         * Initializes new batch such that all Models belonging to same batch
         * can be rendered with single glDraw call.
         * 
         * @param model model to initialize batch from.
         */
        void initialize(ModelBase* model) {}
        /** Initializes OGL context such that model can be rendered.
         * 
         * For Example binding correct OGL Texture target.
         */
        void enter() {}
        /** Reverts changes by @ref exit . */
        void exit() {}
        
        unsigned int offset;
        unsigned int numVertex;
    };

    virtual ~ModelBase() {};
    /** Insert vertex data into respective buffers. */
    virtual void insert(unsigned int offset) = 0;
    virtual unsigned int getNumVertex() = 0;
    /** Compare two models such that glDraw calls can be minimized.
     * 
     * @param a, b models to compare.
     * @returns @code true @endcode if @p b should belong to different @ref Batch than @p a . 
     */
    static bool compare(ModelBase* a, ModelBase* b);
};


/** Implementation of  @ref ModelBase for 3D meshes with texture. */
class Textured3D : public ModelBase {
  public:
    /** Extension of @ref ModelBase::Batch to handle OGL textures. */
    struct Batch : public ModelBase::Batch {
        using ModelBase::Batch::Batch;
        /** Binds correct texture to texture target. */
        void enter();
        /** Removes texture binding. */
        void exit();
        void initialize(Textured3D* model);
        
        GLuint texture;
    };

    static bool compare(Textured3D* a, Textured3D* b);

    /**
     * @param vertexBuffer, uvBuffer access to buffer holding vertex/uv data.
     * @param vertices, uvs vertex/uv values.
     * @param texture texture ID of used texture.
     * @param numVertex number of vertices, not size of @p vertices / @p uvs arrays.
     */
    Textured3D(BufferView* vertexBuffer, GLfloat* vertices, BufferView* uvBuffer, GLfloat* uvs, GLuint texture, unsigned int numVertex);

    void insert(unsigned int offset);
    unsigned int getNumVertex();
    /** @returns ID of used texture. */
    GLuint getTexture();

  private:
    GLfloat* vertices;
    BufferView* vertexBuffer;
    GLfloat* uvs;
    BufferView* uvBuffer;
    GLuint texture;
    unsigned int numVertex;
};