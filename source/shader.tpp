#pragma once


template<typename T>
inline void ShaderProgram::bindUniform(std::string name, const T* values, GLsizei count)
{
    GLuint index = uniformLookup[name].second;
    GLint location = uniformLookup[name].first;

    GLenum type;
    glGetActiveUniform(id, index, 0, nullptr, nullptr, &type, nullptr);

    callback_t callback = uniformCallback<T>(location, type, values, count);

    uniformSetters.push_back(callback);
}


inline void ShaderProgram::bindUniform(
    std::string name, GLboolean transpose, const GLfloat* values, GLsizei count)
{
    GLuint index = uniformLookup[name].second;
    GLint location = uniformLookup[name].first;

    GLenum type;
    glGetActiveUniform(id, index, 0, nullptr, nullptr, &type, nullptr);

    callback_t callback = uniformMatrixCallback(location, type, values, count, transpose);

    uniformSetters.push_back(callback);
}


template<IntConvertable T>
inline callback_t uniformCallback(GLint location, GLenum type, const T* values, GLsizei count)
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
inline callback_t uniformCallback(GLint location, GLenum type, const T* values, GLsizei count)
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
inline callback_t uniformCallback(GLint location, GLenum type, const T* values, GLsizei count)
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


inline callback_t uniformMatrixCallback(
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