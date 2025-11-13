#pragma once

#include <GL/Glew.h>

#include <functional>
#include <glm/glm.hpp>
#include <map>
#include <stdexcept>
#include <type_traits>


/// @brief Compiles and links vertex and fragment shaders.
/// @param vertexSource, fragmentSource null terminated string containing shader code.
/// @return shader program id.
GLuint compileShader(const char* vertexSource, const char* fragmentSource);

/// Provides simplified access to OGL shader API.
class ShaderProgram {
  public:
    using callback_t = std::function<void()>;

    /// @param vertexShader, fragmentShader null terminated string containing shader code.
    ShaderProgram(const char* vertexShader, const char* fragmentShader);

    /// @brief Binds a non-matrix uniform to a value.
    /// Creates a binding between @p values and uniform assigned to @name. If possible values are
    /// converted to type matching uniform variable.
    /// @param name name of uniform variable.
    /// @param values values to bind.
    /// @param count specifies number of uniforms to modify if @p name references uniform array. For
    /// array of size @code N @endcode and uniform variable of size @code M @endcode values should
    /// point to array of size @code N * M @endcode . If binding to non-array uniform should be
    /// @code 1 @endcode .
    template<typename T> void bindUniform(std::string name, const T* values, GLsizei count = 1);
    /// @brief Binds a uniform of matrix type to a value.
    /// @param transpose either @code GL_FALSE @endcode or @code GL_TRUE @endcode . If @code GL_TRUE
    /// @endcode values are transferred to uniform in transposed order.
    void bindUniform(
        std::string name, GLboolean transpose, const GLfloat* values, GLsizei count = 1);
    void registerGLSetting(callback_t set, callback_t unset = nullptr);
    /// Binds shader to ogl context and sets uniforms to bound values.
    void use() const;
    /// Unbinds shader from ogl context.
    void disable() const;

    /// @brief Returns index of vertex attribute.
    /// @param name of vertex attribute.
    GLint getAttribIndex(std::string name) const { return glGetAttribLocation(id, name.c_str()); };
    /// Returns number of vertex attributes.
    unsigned int getNumAttribs() const { return numAttribs; };

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