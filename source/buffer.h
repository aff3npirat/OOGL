#pragma once

#include <GL/Glew.h>

#include <utility>


/// @brief Manages vertex data for OGL Buffer Objects.
struct Buffer {
    Buffer() = default;
    ~Buffer();
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


struct BufferView {
    Buffer* buffer;
    unsigned int stride;
    unsigned int offset;
    unsigned int vertexSize;
    int glType;
};


#include "buffer.tpp"