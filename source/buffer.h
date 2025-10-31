#pragma once

#include <GL/Glew.h>

#include <utility>


/// @brief Manages vertex data for OGL Buffer Objects.
struct Buffer {
    Buffer() = default;
    ~Buffer() { delete ptr; }
    Buffer(Buffer& other) = delete;
    Buffer& operator=(Buffer& other) = delete;

    /// Allocates memory for @p size values.
    template<typename T> void init(std::in_place_type_t<T>, unsigned int size);
    /// @returns size in bytes of single value.
    unsigned int byteSize() const;
    /// @returns number of values in buffer.
    unsigned int size() const;
    /// @returns void pointer to buffer values
    void* data() const;
    /// @returns unique OGL id assigned to a GL Buffer Object.
    unsigned int id() const;

  private:
    struct Base {
        virtual ~Base() {};
        virtual void* getValues() const = 0;

        unsigned int size;
        unsigned int byteSize;
        GLuint id;
    };

    template<typename T> struct Implement : public Base {
        Implement(unsigned int size);
        ~Implement();

        void* getValues() const;

        T* values;
    };

    Base* ptr;
};


/// @brief Strided access to a @ref Buffer instance.
/// Stores a reference to @ref Buffer instance but only has access
/// to a subset of buffer. The referenced buffer is accessed in chunks
/// (here called groups) of consecutive elements with a fixed distance
/// between two groups. For example buffer has 8 elements
/// and a @ref BufferView accesses the groups at indices
/// @code G1=(0, 1, 2), G2=(5, 6, 7) @endcode .
struct BufferView {
    /// @param buffer @ref Buffer to access.
    /// @param stride, vertexSize specifies indices at which @ref Buffer can be accessed.
    /// Buffer is treated as groups of size @p vertexSize and between two consecutive groups are
    /// @p stride number of elements (between first elements of groups, so @p stride - @p vertexSize
    /// equals to number of non-group elements between two consecutive groups).
    /// @param offset number of elements before first group should start.
    /// @param glType OGL Macro specifying type.
    BufferView(Buffer* buffer, unsigned int stride, unsigned int offset, unsigned int vertexSize,
        int glType);
    /// @brief Inserts values into referenced @ref Buffer.
    /// Insertes values at indices specified by stride, vertexSize, offset.
    /// Referenced @ref Buffer object must have at least a size of @code
    /// (stride * (size/vertexSize)) + offset @endcode .
    /// @param values values to insert.
    /// @param size number of values to insert. Must be divisible by @code vertexSize @endcode .
    /// @param tempOffset number of groups to skip. Can be used
    /// to fill referenced @ref Buffer in batches.
    template<typename T>
    void insert(const T* values, unsigned int size, unsigned int tempOffset) const;

    Buffer* buffer;
    unsigned int stride;
    unsigned int offset;
    unsigned int vertexSize;
    int glType;
};


#include "buffer.tpp"