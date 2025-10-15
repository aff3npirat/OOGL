#pragma once

#include <GL/Glew.h>

#include <functional>
#include <glm/glm.hpp>
#include <map>
#include <stdexcept>
#include <type_traits>


enum UNIFORM_TYPE { SCALAR, VEC2, VEC3, VEC4 };
enum MATDIM { MAT2x2, MAT2x3, MAT2x4, MAT3x2, MAT3x3, MAT3x4, MAT4x2, MAT4x3, MAT4x4 };

template<typename T> concept UniformScalarType =
    (std::is_same<T, GLint>::value || std::is_same<T, GLfloat>::value ||
     std::is_same<T, GLuint>::value);

class ShaderProgram {
   public:
    static void compileShader(const char* vertexSource, const char* fragmentSource);

    ShaderProgram(
        const char* vertexShader, const char* fragmentShader, unsigned int* attribs,
        unsigned int numAttribs);

    void use();
    void disable();

    template<UniformScalarType T>
    void setUniform(std::string location, T* v0, T* v1 = nullptr, T* v2 = nullptr, T* v3 = nullptr);
    template<UniformScalarType T>
    void setUniformArray(std::string location, GLsizei count, T* values, UNIFORM_TYPE uniformType);
    template<UniformScalarType T>
    void setUniformMatrixArray(
        std::string location, GLsizei count, GLboolean transpose, T* values, MATDIM dim);

   private:
    template<UniformScalarType T>
    void setUniform(GLint location, T* v0, T* v1 = nullptr, T* v2 = nullptr, T* v3 = nullptr);
    template<UniformScalarType T>
    void setUniformArray(GLint location, GLsizei, T* values, UNIFORM_TYPE uniformType);
    template<UniformScalarType T>
    void setUniformMatrixArray(
        GLint location, GLsizei count, GLboolean transpose, T* values, MATDIM dim);

    void setupContext();
    void undoContext();

    GLuint id;
    unsigned int* attribs;
    unsigned int numAttribs;
    std::vector<std::function<void()> > uniformSetters;
};


template<UniformScalarType T>
inline void ShaderProgram::setUniform(std::string location, T* v0, T* v1, T* v2, T* v3)
{
    setUniform(glGetUniformLocation(id, location.c_str()), v0, v1, v2, v3);
}


template<UniformScalarType T>
inline void ShaderProgram::setUniformArray(
    std::string location, GLsizei count, T* values, UNIFORM_TYPE uniformType)
{
    setUniformArray(glGetUniformLocation(id, location.c_str()), count, values, uniformType);
}


template<UniformScalarType T>
inline void ShaderProgram::setUniformMatrixArray(
    std::string location, GLsizei count, GLboolean transpose, T* values, MATDIM dim)
{
    setUniformMatrixArray(
        glGetUniformLocation(id, location.c_str()), count, transpose, values, dim);
}


template<UniformScalarType T>
inline void ShaderProgram::setUniform(GLint location, T* v0, T* v1, T* v2, T* v3)
{
    UNIFORM_TYPE version = UNIFORM_TYPE::SCALAR;
    if (v1 != nullptr) {
        version = UNIFORM_TYPE::VEC2;
        if (v2 != nullptr) {
            version = UNIFORM_TYPE::VEC3;
            if (v3 != nullptr) {
                version = UNIFORM_TYPE::VEC4;
            }
        }
    }

    std::function<void()> callback;
    if constexpr (std::is_same<T, GLint>::value || std::is_same<T, GLboolean>::value) {
        switch (version) {
            case UNIFORM_TYPE::SCALAR:
                callback = [=]() { glUniform1i(location, *v0); };
            case UNIFORM_TYPE::VEC2:
                callback = [=]() { glUniform2i(location, *v0, *v1); };
            case UNIFORM_TYPE::VEC3:
                callback = [=]() { glUniform3i(location, *v0, *v1, *v2); };
            case UNIFORM_TYPE::VEC4:
                callback = [=]() { glUniform4i(location, *v0, *v1, *v2, *v3); };
        }
    }
    else if constexpr (std::is_same<T, GLfloat>::value) {
        switch (version) {
            case UNIFORM_TYPE::SCALAR:
                callback = [=]() { glUniform1f(location, *v0); };
            case UNIFORM_TYPE::VEC2:
                callback = [=]() { glUniform2f(location, *v0, *v1); };
            case UNIFORM_TYPE::VEC3:
                callback = [=]() { glUniform3f(location, *v0, *v1, *v2); };
            case UNIFORM_TYPE::VEC4:
                callback = [=]() { glUniform4f(location, *v0, *v1, *v2, *v3); };
        }
    }
    else if constexpr (std::is_same<T, GLuint>::value) {
        switch (version) {
            case UNIFORM_TYPE::SCALAR:
                callback = [=]() { glUniform1ui(location, *v0); };
            case UNIFORM_TYPE::VEC2:
                callback = [=]() { glUniform2ui(location, *v0, *v1); };
            case UNIFORM_TYPE::VEC3:
                callback = [=]() { glUniform3ui(location, *v0, *v1, *v2); };
            case UNIFORM_TYPE::VEC4:
                callback = [=]() { glUniform4ui(location, *v0, *v1, *v2, *v3); };
        }
    }

    uniformSetters.push_back(callback);
}


template<UniformScalarType T>
inline void ShaderProgram::setUniformArray(
    GLint location, GLsizei count, T* values, UNIFORM_TYPE uniformType)
{
    std::function<void()> callback;
    if constexpr (std::is_same<T, GLint>::value || std::is_same<T, GLboolean>::value) {
        switch (uniformType) {
            case UNIFORM_TYPE::SCALAR:
                callback = [=]() { glUniform1iv(location, count, values); };
            case UNIFORM_TYPE::VEC2:
                callback = [=]() { glUniform2iv(location, count, values); };
            case UNIFORM_TYPE::VEC3:
                callback = [=]() { glUniform3iv(location, count, values); };
            case UNIFORM_TYPE::VEC4:
                callback = [=]() { glUniform4iv(location, count, values); };
        }
    }
    else if constexpr (std::is_same<T, GLfloat>::value) {
        switch (uniformType) {
            case UNIFORM_TYPE::SCALAR:
                callback = [=]() { glUniform1fv(location, count, values); };
            case UNIFORM_TYPE::VEC2:
                callback = [=]() { glUniform2fv(location, count, values); };
            case UNIFORM_TYPE::VEC3:
                callback = [=]() { glUniform3fv(location, count, values); };
            case UNIFORM_TYPE::VEC4:
                callback = [=]() { glUniform4fv(location, count, values); };
        }
    }
    else if constexpr (std::is_same<T, GLuint>::value) {
        switch (uniformType) {
            case UNIFORM_TYPE::SCALAR:
                callback = [=]() { glUniform1uiv(location, count, values); };
            case UNIFORM_TYPE::VEC2:
                callback = [=]() { glUniform2uiv(location, count, values); };
            case UNIFORM_TYPE::VEC3:
                callback = [=]() { glUniform3uiv(location, count, values); };
            case UNIFORM_TYPE::VEC4:
                callback = [=]() { glUniform4uiv(location, count, values); };
        }
    }

    uniformSetters.push_back(callback);
}


template<UniformScalarType T>
inline void ShaderProgram::setUniformMatrixArray(
    GLint location, GLsizei count, GLboolean transpose, T* values, MATDIM dim)
{
    std::function<void()> callback;
    switch (dim) {
        case MATDIM::MAT2x2:
            callback = [=]() { glUniformMatrix2fv(location, count, transpose, values); };
        case MATDIM::MAT2x3:
            callback = [=]() { glUniformMatrix2x3fv(location, count, transpose, values); };
        case MATDIM::MAT2x4:
            callback = [=]() { glUniformMatrix2x4fv(location, count, transpose, values); };
        case MATDIM::MAT3x2:
            callback = [=]() { glUniformMatrix3x2fv(location, count, transpose, values); };
        case MATDIM::MAT3x3:
            callback = [=]() { glUniformMatrix3fv(location, count, transpose, values); };
        case MATDIM::MAT3x4:
            callback = [=]() { glUniformMatrix3x4fv(location, count, transpose, values); };
        case MATDIM::MAT4x2:
            callback = [=]() { glUniformMatrix4x2fv(location, count, transpose, values); };
        case MATDIM::MAT4x3:
            callback = [=]() { glUniformMatrix4x3fv(location, count, transpose, values); };
        case MATDIM::MAT4x4:
            callback = [=]() { glUniformMatrix4fv(location, count, transpose, values); };
    }

    uniformSetters.push_back(callback);
}
