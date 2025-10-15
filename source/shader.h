#pragma once

#include <GL/Glew.h>

#include <functional>
#include <glm/glm.hpp>
#include <map>
#include <stdexcept>
#include <type_traits>


template<typename T> concept UniformScalar =
    (std::is_same<T, GLint>::value || std::is_same<T, GLfloat>::value ||
     std::is_same<T, GLuint>::value || std::is_same<T, GLboolean>::value);

class ShaderProgram {
  public:
    static void compileShader(const char* vertexSource, const char* fragmentSource);

    ShaderProgram(
        const char* vertexShader, const char* fragmentShader, unsigned int* attribs,
        unsigned int numAttribs);

    void use();
    void disable();

    template<UniformScalar T> void setUniform(std::string name, T* values, GLsizei count = 1);
    template<UniformScalar T>
    void setUniform(std::string name, T* values, GLboolean transpose, GLsizei count = 1);

  private:
    template<UniformScalar T>
    void _setUniform(std::string name, T* values, GLsizei count, GLboolean transpose);

    void setupContext();
    void undoContext();

    GLuint id;
    unsigned int* attribs;
    unsigned int numAttribs;
    std::vector<std::function<void()>> uniformSetters;
    std::map<std::string, std::pair<GLint, GLuint>>
        uniformLookup;  // Maps name to (location, index)
};


template<UniformScalar T>
inline void ShaderProgram::setUniform(std::string name, T* values, GLsizei count)
{
    _setUniform(name, values, count, GL_FALSE);
}


template<UniformScalar T>
inline void ShaderProgram::setUniform(
    std::string name, T* values, GLboolean transpose, GLsizei count)
{
    _setUniform(name, values, count, transpose);
}


template<UniformScalar T>
inline void ShaderProgram::_setUniform(
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

    void (*callback)(GLint, GLsizei, GLboolean, T*);
    switch (type) {
        // MATRICES
        case GL_FLOAT_MAT2: callback = glUniformMatrix2fv;
        case GL_FLOAT_MAT2x3: callback = glUniformMatrix2x3fv;
        case GL_FLOAT_MAT2x4: callback = glUniformMatrix2x4fv;
        case GL_FLOAT_MAT3x2: callback = glUniformMatrix3x2fv;
        case GL_FLOAT_MAT3: callback = glUniformMatrix3fv;
        case GL_FLOAT_MAT3x4: callback = glUniformMatrix3x4fv;
        case GL_FLOAT_MAT4x2: callback = glUniformMatrix4x2fv;
        case GL_FLOAT_MAT4x3: callback = glUniformMatrix4x3fv;
        case GL_FLOAT_MAT4: callback = glUniformMatrix4fv;
    }

    uniformSetters.push_back([=]() { callback(location, count, transpose, values); });
}