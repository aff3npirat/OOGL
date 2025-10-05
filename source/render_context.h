#pragma once

#include <utility>
#include <vector>

#include <GL/Glew.h>


struct Buffer {
    struct Base {
        virtual ~Base() = 0;

        unsigned int size;
        void* values;
    };

    template<typename T>
    struct Implement : Base {
        Implement(unsigned int size) {
            values = new T[size];
            this->size = size;
        }
        ~Implement() { delete[] values; }

        T* values;
        unsigned int size;
        GLuint uid;
    };

    ~Buffer() { delete ptr; }
    template<typename T> void init(std::in_place_type_t<T>, unsigned int size) { ptr = new Implement<T>(size); }
    unsigned int byteSize() { return ptr->size; }
    void* data() { return (void*)ptr->values; }

    Base* ptr;
};


class BufferView {
  public:
    BufferView(unsigned int stride, unsigned int offset, unsigned int vertexSize, int glType) {
        this->stride = stride;
        this->offset = offset;
        this->vertexSize = vertexSize;
        this->glType = glType;
    }

    template<typename T>
    void insert(T* values, unsigned int size, Buffer buf) {
        T* first = static_cast<T*>(buf.data())[offset];

        for (int i = 0; i < size; i += vertexSize) {
            for (int j = 0; j < vertexSize; j++) {
                first[i*stride + j] = values[i + j];
            }
        }
    }   

  private:
    unsigned int stride;
    unsigned int offset;
    unsigned int vertexSize;
    int glType;
};
