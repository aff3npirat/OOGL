#pragma once

#include <GL/Glew.h>

#include <functional>
#include <glm/glm.hpp>
#include <map>
#include <stdexcept>
#include <type_traits>


GLuint compileShader(const char* vertexSource, const char* fragmentSource);

/** Provides simplified access to OGL shader API. */
class ShaderProgram {
  public:
    using callback_t = std::function<void()>;

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
        uniformLookup;    // Maps name to (location, index)
};


#include "shader.tpp"