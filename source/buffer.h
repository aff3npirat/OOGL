#pragma once

#include <GL/Glew.h>

#include <utility>


/// @brief Manages vertex data for OGL Buffer Objects.
struct Buffer {
    Buffer();
    ~Buffer();
    Buffer(Buffer& other) = delete;
    Buffer& operator=(Buffer& other) = delete;

    /// Allocates memory for @p size values.
    template<typename T> void resize(std::in_place_type_t<T>, unsigned int size);
    /// @returns size in bytes of single value.
    unsigned int getByteSize() const { return byteSize; }
    /// @returns number of values in buffer.
    unsigned int getSize() const { return size; };
    /// @returns void pointer to buffer values
    void* data() const { return ptr->getValues(); };
    /// @returns unique OGL id assigned to a GL Buffer Object.
    unsigned int getId() const { return id; };

  private:
    struct Base {
        virtual ~Base() {};
        virtual void* getValues() const = 0;
    };

    template<typename T> struct Implement : public Base {
        Implement(unsigned int size);
        ~Implement();

        void* getValues() const { return (void*)values; };

        T* values;
    };

    Base* ptr;
    unsigned int size;
    unsigned int byteSize;
    GLuint id;
};


struct BufferView {
    Buffer* buffer;
    unsigned int stride;
    unsigned int offset;
    unsigned int vertexSize;
    int glType;
};


#include "buffer.tpp"