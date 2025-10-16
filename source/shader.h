#pragma once

#include <GL/Glew.h>

#include <functional>
#include <glm/glm.hpp>
#include <map>
#include <stdexcept>
#include <type_traits>


namespace {
typedef void (*callback_t)();

template<typename T> concept UniformScalar =
    (std::is_same<T, GLint>::value || std::is_same<T, GLfloat>::value ||
        std::is_same<T, GLuint>::value || std::is_same<T, GLboolean>::value);
}  // namespace


GLuint compileShader(const char* vertexSource, const char* fragmentSource);


/** Provides simplified access to OGL shader API. */
class ShaderProgram {
  public:
    ShaderProgram(const char* vertexShader, const char* fragmentShader);

    /** Binds a non-matrix uniform to a value. */
    template<UniformScalar T>
    void bindUniform(std::string name, T* values, GLsizei count = 1);
    /** Binds a uniform of type matrix to a value. */
    template<UniformScalar T>
    void bindUniform(std::string name, T* values, GLboolean transpose, GLsizei count = 1);
    /**
     * @param name name of attribute variable.
     * @returns index of an attribute variable.
     */
    GLint getAttribIndex(std::string name);
    void registerGLSetting(callback_t set, callback_t unset = nullptr);
    /** Binds shader to ogl context and enables/sets attributes/uniforms. */
    void use();
    /** Unbinds shader from ogl context. */
    void disable();

  private:
    /** Binds a uniform variable to a value.
     *
     * @param name variable name in GLSL Shader program.
     * @param values value to bind uniform to. The value will be evaluated when @ref
     * ShaderProgram#use is called. When setting non-scalar uniforms should point to first address
     * of tightly packed values, same for when setting uniform arrays.
     * @param count for uniform arrays specifies number of elements to set. Should be 1 for
     * non-arrays.
     * @param transpose when setting uniform of matrix type should be @code true @endcode if values
     * should be read in transposed order. Ignored for non matrix uniforms.
     */
    template<UniformScalar T>
    void _bindUniform(std::string name, T* values, GLsizei count, GLboolean transpose);

    GLuint id;
    unsigned int numAttribs;
    std::vector<std::function<void()>> uniformSetters;
    callback_t oglSetting;
    callback_t unsetOGLSetting;
    std::map<std::string, std::pair<GLint, GLuint>>
        uniformLookup;  // Maps name to (location, index)
};


template<UniformScalar T>
inline void ShaderProgram::bindUniform(std::string name, T* values, GLsizei count)
{
    _bindUniform(name, values, count, GL_FALSE);
}


template<UniformScalar T>
inline void ShaderProgram::bindUniform(
    std::string name, T* values, GLboolean transpose, GLsizei count)
{
    _bindUniform(name, values, count, transpose);
}


template<UniformScalar T>
inline void ShaderProgram::_bindUniform(
    std::string name, T* values, GLsizei count, GLboolean transpose)
{
    GLuint index = uniformLookup[name].second;
    GLint location = uniformLookup[name].first;

    GLenum type;
    glGetActiveUniform(id, index, 0, nullptr, nullptr, &type, nullptr);

    void (*callback)(GLint, GLsizei, T*);
    bool isNoMatrix = true;
    switch (type) {
        // SCALARS
        case GL_FLOAT: callback = glUniform1fv;
        case GL_INT: callback = glUniform1iv;
        case GL_UNSIGNED_INT || GL_BOOL: callback = glUniform1uiv;
        // VEC2
        case GL_FLOAT_VEC2: callback = glUniform2fv;
        case GL_INT_VEC2: callback = glUniform2iv;
        case GL_UNSIGNED_INT_VEC2 || GL_BOOL_VEC2: callback = glUniform2uiv;
        // VEC3
        case GL_FLOAT_VEC3: callback = glUniform3fv;
        case GL_INT_VEC3: callback = glUniform3iv;
        case GL_UNSIGNED_INT_VEC3 || GL_BOOL_VEC3: callback = glUniform3uiv;
        // VEC4
        case GL_FLOAT_VEC4: callback = glUniform4fv;
        case GL_INT_VEC4: callback = glUniform4iv;
        case GL_UNSIGNED_INT_VEC4 || GL_BOOL_VEC4: callback = glUniform4uiv;
        default: isNoMatrix = false;
    }

    if (isNoMatrix) {
        uniformSetters.push_back([=]() { callback(location, count, values); });
        return;
    }

    void (*matrix_callback)(GLint, GLsizei, GLboolean, T*);
    switch (type) {
        // MATRICES
        case GL_FLOAT_MAT2: matrix_callback = glUniformMatrix2fv;
        case GL_FLOAT_MAT2x3: matrix_callback = glUniformMatrix2x3fv;
        case GL_FLOAT_MAT2x4: matrix_callback = glUniformMatrix2x4fv;
        case GL_FLOAT_MAT3x2: matrix_callback = glUniformMatrix3x2fv;
        case GL_FLOAT_MAT3: matrix_callback = glUniformMatrix3fv;
        case GL_FLOAT_MAT3x4: matrix_callback = glUniformMatrix3x4fv;
        case GL_FLOAT_MAT4x2: matrix_callback = glUniformMatrix4x2fv;
        case GL_FLOAT_MAT4x3: matrix_callback = glUniformMatrix4x3fv;
        case GL_FLOAT_MAT4: matrix_callback = glUniformMatrix4fv;
    }

    uniformSetters.push_back([=]() { matrix_callback(location, count, transpose, values); });
}