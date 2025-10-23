#pragma once

#include <GL/Glew.h>

#include <functional>
#include <glm/glm.hpp>
#include <map>
#include <stdexcept>
#include <type_traits>


namespace {
typedef std::function<void()> callback_t;

template<typename T> concept UniformScalar =
    (std::is_same<T, GLint>::value || std::is_same<T, GLfloat>::value ||
        std::is_same<T, GLuint>::value || std::is_same<T, GLboolean>::value);

template<typename T> concept FloatConvertable = std::is_same<T, GLfloat>::value;
template<typename T> concept IntConvertable = std::is_same<T, GLint>::value;
template<typename T> concept UIntConvertable =
    (std::is_same<T, GLuint>::value || std::is_same<T, GLboolean>::value);

}  // namespace

/**
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
static callback_t uniformCallback(GLint location, GLenum type, const T* values, GLsizei count);
template<IntConvertable T>
static callback_t uniformCallback(GLint location, GLenum type, const T* values, GLsizei count);
template<UIntConvertable T>
static callback_t uniformCallback(GLint location, GLenum type, const T* values, GLsizei count);

/** Binds a uniform of matrix type to a value.
 *
 * @param location, type, values, count @sa uniformCallback
 * @param transpose @code GL_TRUE @endcode if values should be read in transposed order, so
 * first value is stored in element with highest index.
 *
 * @returns @sa uniformCallback
 */
static callback_t uniformMatrixCallback(
    GLint location, GLenum type, const GLfloat* values, GLsizei count, GLboolean transpose);

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
    GLuint id;
    unsigned int numAttribs;
    std::vector<callback_t> uniformSetters;
    callback_t oglSetting;
    callback_t unsetOGLSetting;
    std::map<std::string, std::pair<GLint, GLuint>>
        uniformLookup;  // Maps name to (location, index)
};


#include "shader.tpp"