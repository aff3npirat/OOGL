#pragma once

#include <algorithm>
#include <map>
#include <utility>
#include <vector>

#include <GL/Glew.h>

#include "model.h"


/** Wrapper for template to enable duck typing.
 * 
 * Maps an OGL Buffer (by uid) to array of arbitrary type. The actual 
 * type is hidden.
 */
struct Buffer {
    struct Base {
        virtual ~Base() = 0;
        virtual void* getValues() = 0;

        unsigned int size;
        unsigned int byteSize;
        GLuint uid;
    };

    template<typename T>
    struct Implement : Base {
        Implement(unsigned int size) {
            values = new T[size];
            this->size = size;
            byteSize = sizeof(T);

            glGenBuffers(1, &uid);
        }
        ~Implement() { delete[] values; }

        void* getValues() { return (void*)values; }

        T* values;
    };

    ~Buffer() { delete ptr; }
    
    /** Allocates new array of size @p size */
    template<typename T> void init(std::in_place_type_t<T>, unsigned int size) { ptr = new Implement<T>(size); }
    
    /** @returns Size in bytes of single value of array. */
    unsigned int byteSize() { return ptr->byteSize; }
    
    /** @returns Number of values in buffer. */
    unsigned int size() { return ptr->size; }
    
    /** @returns Void pointer to array. */
    void* data() { return ptr->getValues(); }

    /** @returns unique OGL id assigned to a GL Buffer Object. */
    unsigned int uid() { return ptr->uid; }

  private:
    Base* ptr;
};


/** Strided access to a @ref Buffer instance.
 * 
 * Stores a reference to @ref Buffer instance but only has access
 * to a subset of array. The referenced Buffer is accessed in chunks
 * (here called groups) of consecutive elements with a fixed distance
 * between two groups. For example the referenced Buffer has 8 elements
 * and a @ref BufferView accesses the groups at indices
 * @code G1=(0, 1, 2), G2=(5, 6, 7) @endcode .
 */
struct BufferView {
    /**
     * @param buffer @ref Buffer to access.
     * @param stride, vertexSize specifies indices at which @ref Buffer can be accessed.
     * Buffer is treated as groups of size @p vertexSize and between two consecutive groups are 
     * @p stride number of elements (between first elements of groups, so @p stride - @p vertexSize
     * equals to number of non-group elements between two consecutive groups).
     * @param offset number of elements before first group starts.
     * @param glType macros defined by OGL.
     */
    BufferView(Buffer* buffer, unsigned int stride, unsigned int offset, unsigned int vertexSize, int glType) {
        this->buffer = buffer;
        this->stride = stride;
        this->offset = offset;
        this->vertexSize = vertexSize;
        this->glType = glType;
    }

    /** Insert values into viewed buffer.
     * 
     * Referenced @ref Buffer object must have at least a size of @code
     * (stride * (size/vertexSize)) + offset @endcode .
     * 
     * @param values values to insert.
     * @param size number of values to insert. Must be divisible by @code vertexSize @endcode .
     * @param tempOffset additional offset from from first group. Can be used
     * to fill referenced @ref Buffer batchwise.
     */
    template<typename T>
    void insert(T* values, unsigned int size, unsigned int tempOffset) {
        T* first = static_cast<T*>(buffer->data()) + offset + tempOffset;

        int idx = 0;
        int stridedIdx = 0;
        while (idx < size) {
            for (int j = 0; j < vertexSize; j++) {
                first[stridedIdx + j] = values[idx + j];
            }

            idx += vertexSize;
            stridedIdx += stride;
        }
    }

    Buffer* buffer;  /**< Reference to accessed @ref Buffer. */
    unsigned int stride;  /**< Number of values between consecutive groups. */
    unsigned int offset;  /**< Number of values before first group. */
    unsigned int vertexSize;  /**< Size of a group. */
    int glType;
};


template<class C>
concept IsModelBaseSubClass = std::is_base_of<ModelBase, C>::value;

/** Manages VAO initialization and data transfer to GPU. */
template<IsModelBaseSubClass Model>
class RenderContext {
  public:
    /**
     * @param bufferViews each view is linked to specific shader attribute.
     * @param shaderAttribs shader attributes linked to @p bufferViews .
     * @param size number of @p bufferViews and @p shaderAttribs . 
     */
    RenderContext(BufferView* bufferViews, unsigned int* shaderAttribs, unsigned int size) {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        
        std::map<GLuint, Buffer*> buffers;
        for (int i = 0; i < size; i++) {
            this->shaderAttribs[i] = shaderAttribs[i];

            glBindBuffer(GL_ARRAY_BUFFER, bufferViews[i].buffer->uid());
            glVertexAttribPointer(
                shaderAttribs[i],
                bufferViews[i].numVertex,
                bufferViews[i].glType,
                GL_FALSE,
                bufferViews[i].stride * bufferViews[i].buffer->byteSize(),
                bufferViews[i].offset * bufferViews[i].buffer->byteSize()
            );

            if (!buffers.contains(bufferViews[i].buffer->uid)) {
                buffers[bufferViews[i].buffer->uid()] = bufferViews[i].buffer;
            }
        }
        numAttribs = size;

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        numBuffers = buffers.size();
        this->buffers = new Buffer*[numBuffers];
        Buffer** it = buffers.begin();
        for (int i = 0; i < numBuffers; i++) {
            this->buffers[i] = it[i];
        }
    }
    ~RenderContext() { delete[] buffers; }

    /** Stores model reference to render.
     * 
     * This Method does not render any models and also does
     * not store any data. All changes to @p model will be rendererd
     * when @ref RenderContext#render is called.
     */
    void addModel(Model& model) { models.push_back(model); }
    void render() {}

  private:
    GLuint vao;
    Buffer** buffers;
    unsigned int numBuffers;
    unsigned int* shaderAttribs;
    unsigned int numAttribs;
    std::vector<Model*> models;
};
