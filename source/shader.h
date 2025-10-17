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

template<typename T> concept FloatConvertable = std::is_same<T, GLfloat>::value;
template<typename T> concept IntConvertable = std::is_same<T, GLint>::value;
template<typename T> concept UIntConvertable =
    (std::is_same<T, GLuint>::value || std::is_same<T, GLboolean>::value);
}  // namespace


GLuint compileShader(const char* vertexSource, const char* fragmentSource);


/** Provides simplified access to OGL shader API. */
class ShaderProgram {
  public:
    ShaderProgram(const char* vertexShader, const char* fragmentShader);

    /** Binds a non-matrix uniform to a value. */
    template<typename T>
    void bindUniform(std::string name, const T* values, GLsizei count = 1);
    /** Binds a uniform of matrix type to a value. */
    void bindUniform(
        std::string name, GLboolean transpose, const GLfloat* values, GLsizei count = 1);
    /**
     * @param name name of attribute variable.
     * @returns index of an attribute variable.
     */
    GLint getAttribIndex(std::string name);
    unsigned int getNumAttribs();
    void registerGLSetting(callback_t set, callback_t unset = nullptr);
    /** Binds shader to ogl context and enables/sets attributes/uniforms. */
    void use();
    /** Unbinds shader from ogl context. */
    void disable();

  private:
    /** Binds a non-matrix uniform variable to a value.
     *
     * @param location variable id in GLSL Shader program.
     * @param type type of uniform variable.
     * @param values value to bind uniform to. The value will be evaluated when @ref
     * ShaderProgram#use is called. When setting arrays of uniforms should point to first
     * index of tightly packed values.
     * @param count for uniform arrays specifies number of elements to set. Should be 1 for
     * non-arrays.
     *
     * @returns lambda expression which will send values to uniform variable on call.
     */
    template<FloatConvertable T>
    std::function<void()> uniformCallback(
        GLint location, GLenum type, const T* values, GLsizei count);
    template<IntConvertable T>
    std::function<void()> uniformCallback(
        GLint location, GLenum type, const T* values, GLsizei count);
    template<UIntConvertable T>
    std::function<void()> uniformCallback(
        GLint location, GLenum type, const T* values, GLsizei count);
    /** Binds a uniform of matrix type to a value.
     *
     * @param location, type, values, count @sa uniformCallback
     * @param transpose @code GL_TRUE @endcode if values should be read in transposed order, so
     * first value is stored in element with highest index.
     *
     * @returns @sa uniformCallback
     */
    std::function<void()> uniformMatrixCallback(
        GLint location, GLenum type, const GLfloat* values, GLsizei count, GLboolean transpose);

    GLuint id;
    unsigned int numAttribs;
    std::vector<std::function<void()>> uniformSetters;
    callback_t oglSetting;
    callback_t unsetOGLSetting;
    std::map<std::string, std::pair<GLint, GLuint>>
        uniformLookup;  // Maps name to (location, index)
};


template<typename T>
inline void ShaderProgram::bindUniform(std::string name, const T* values, GLsizei count)
{
    GLuint index = uniformLookup[name].second;
    GLint location = uniformLookup[name].first;

    GLenum type;
    glGetActiveUniform(id, index, 0, nullptr, nullptr, &type, nullptr);

    std::function<void()> callback = uniformCallback<T>(location, type, values, count);

    uniformSetters.push_back(callback);
}


inline void ShaderProgram::bindUniform(
    std::string name, GLboolean transpose, const GLfloat* values, GLsizei count)
{
    GLuint index = uniformLookup[name].second;
    GLint location = uniformLookup[name].first;

    GLenum type;
    glGetActiveUniform(id, index, 0, nullptr, nullptr, &type, nullptr);

    std::function<void()> callback = uniformMatrixCallback(location, type, values, count, transpose);

    uniformSetters.push_back(callback);
}


template<IntConvertable T>
inline std::function<void()> ShaderProgram::uniformCallback(
    GLint location, GLenum type, const T* values, GLsizei count)
{
    void (*callback)(GLint, GLsizei, const GLint*);

    switch (type) {
        case GL_SAMPLER_2D:
        case GL_INT: callback = glUniform1iv; break;
        case GL_INT_VEC2: callback = glUniform2iv; break;
        case GL_INT_VEC3: callback = glUniform3iv; break;
        case GL_INT_VEC4: callback = glUniform4iv; break;
    }

    // TODO: perform type casting/checking

    return [=]() { callback(location, count, values); };
}


template<FloatConvertable T>
inline std::function<void()> ShaderProgram::uniformCallback(
    GLint location, GLenum type, const T* values, GLsizei count)
{
    void (*callback)(GLint, GLsizei, const GLfloat*);

    switch (type) {
        case GL_FLOAT: callback = glUniform1fv; break;
        case GL_FLOAT_VEC2: callback = glUniform2fv; break;
        case GL_FLOAT_VEC3: callback = glUniform3fv; break;
        case GL_FLOAT_VEC4: callback = glUniform4fv;
    }

    // TODO: perform type casting/checking

    return [=]() { calback(location, count, values); };
}


template<UIntConvertable T>
inline std::function<void()> ShaderProgram::uniformCallback(
    GLint location, GLenum type, const T* values, GLsizei count)
{
    void (*callback)(GLint, GLsizei, const GLuint*);

    switch (type) {
        case GL_BOOL:
        case GL_UNSIGNED_INT: callback = glUniform1uiv; break;
        case GL_BOOL_VEC2:
        case GL_UNSIGNED_INT_VEC2: callback = glUniform2uiv; break;
        case GL_BOOL_VEC3:
        case GL_UNSIGNED_INT_VEC3: callback = glUniform3uiv; break;
        case GL_BOOL_VEC4:
        case GL_UNSIGNED_INT_VEC4: callback = glUniform4uiv;
    }

    // TODO: perform type casting/checking

    return [=]() { calback(location, count, values); };
}


inline std::function<void()> ShaderProgram::uniformMatrixCallback(
    GLint location, GLenum type, const GLfloat* values, GLsizei count, GLboolean transpose)
{
    void (*callback)(GLint, GLsizei, GLboolean, const GLfloat*);
    switch (type) {
        // MATRICES
        case GL_FLOAT_MAT2: callback = glUniformMatrix2fv; break;
        case GL_FLOAT_MAT2x3: callback = glUniformMatrix2x3fv; break;
        case GL_FLOAT_MAT2x4: callback = glUniformMatrix2x4fv; break;
        case GL_FLOAT_MAT3x2: callback = glUniformMatrix3x2fv; break;
        case GL_FLOAT_MAT3: callback = glUniformMatrix3fv; break;
        case GL_FLOAT_MAT3x4: callback = glUniformMatrix3x4fv; break;
        case GL_FLOAT_MAT4x2: callback = glUniformMatrix4x2fv; break;
        case GL_FLOAT_MAT4x3: callback = glUniformMatrix4x3fv; break;
        case GL_FLOAT_MAT4: callback = glUniformMatrix4fv; break;
    }

    // TODO: perform type checking/casting

    return [=]() { callback(location, count, transpose, values); };
}