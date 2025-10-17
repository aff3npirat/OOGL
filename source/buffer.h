#pragma once

#include <GL/Glew.h>

#include <utility>


/** Wrapper for template to enable duck typing.
 *
 * Maps an OGL Buffer (by id) to array of arbitrary type. The actual
 * type is hidden.
 */
struct Buffer {
    struct Base {
        virtual ~Base() {};
        virtual void* getValues() = 0;

        unsigned int size;
        unsigned int byteSize;
        GLuint id;
    };

    template<typename T>
    struct Implement : public Base {
        Implement(unsigned int size)
        {
            values = new T[size];
            this->size = size;
            byteSize = sizeof(T);

            glGenBuffers(1, &id);
        }
        ~Implement() { delete[] values; }

        void* getValues() { return (void*)values; }

        T* values;
    };

    ~Buffer() { delete ptr; }

    /** Allocates new array of size @p size */
    template<typename T>
    void init(std::in_place_type_t<T>, unsigned int size)
    {
        ptr = new Implement<T>(size);
    }

    /** @returns Size in bytes of single value of array. */
    unsigned int byteSize();

    /** @returns Number of values in buffer. */
    unsigned int size();

    /** @returns Void pointer to array. */
    void* data();

    /** @returns unique OGL id assigned to a GL Buffer Object. */
    unsigned int id();

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
    BufferView(Buffer* buffer, unsigned int stride, unsigned int offset, unsigned int vertexSize,
        int glType);

    /** Insert values into viewed buffer.
     *
     * Referenced @ref Buffer object must have at least a size of @code
     * (stride * (size/vertexSize)) + offset @endcode .
     *
     * @param values values to insert.
     * @param size number of values to insert. Must be divisible by @code vertexSize @endcode .
     * @param tempOffset additional offset of elements from first group. Can be used
     * to fill referenced @ref Buffer batchwise.
     */
    template<typename T>
    void insert(T* values, unsigned int size, unsigned int tempOffset)
    {
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

    Buffer* buffer;          /**< Reference to accessed @ref Buffer. */
    unsigned int stride;     /**< Number of values between consecutive groups. */
    unsigned int offset;     /**< Number of values before first group. */
    unsigned int vertexSize; /**< Size of a group. */
    int glType;
};
