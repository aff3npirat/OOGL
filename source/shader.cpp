#include "shader.h"


GLuint ShaderProgram::compileShader(const char* vertexSource, const char* fragmentSource)
{
    printf("Compiling vertex shader\n");
    GLuint vertexID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexID, 1, &vertexSource, NULL);
    glCompileShader(vertexID);

    GLint status = false;
    auto checkShader = [&status](GLuint shaderID) {
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE) {
            GLint logLen;
            glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLen);
            std::vector<char> log(logLen + 1);
            GLsizei written;
            glGetShaderInfoLog(shaderID, logLen, &written, log.data());
            printf("compile error:\n%s", log.data());
        }
    };
    
    checkShader(vertexID);

    printf("Compiling fragment shader\n");
    GLuint fragmentID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(fragmentID, 1, &fragmentSource, NULL);
    glCompileShader(fragmentID);
    checkShader(fragmentID);

    GLuint programID = glCreateProgram();
    glAttachShader(programID, vertexID);
    glAttachShader(programID, fragmentID);
    glLinkProgram(programID);

    glGetProgramiv(programID, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        GLint logLen;
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logLen);
        std::vector<char> log(logLen + 1);
        GLsizei written;
        glGetProgramInfoLog(programID, logLen, &written, log.data());
        printf("link error:\n%s", log.data());
    }

    return programID;
}


ShaderProgram::ShaderProgram(const char* vertexShader, const char* fragmentShader)
{
    id = compileShader(vertexShader, fragmentShader);

    GLint queryResult;
    glGetProgramiv(id, GL_ACTIVE_ATTRIBUTES, &queryResult);
    this->numAttribs = (unsigned int)queryResult;

    GLint maxLength;
    glGetProgramiv(id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLength);
    char* nameBuf = new char[maxLength];
    GLsizei length;

    glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &queryResult);
    for (GLint i = 0; i < queryResult; i++) {
        glGetActiveUniform(id, i, maxLength, &length, nullptr, nullptr, nameBuf);
        
        std::string name(nameBuf, length);
        GLint location = glGetUniformLocation(id, name.c_str());

        uniformLookup[name] = std::make_pair(location, i);
    }

    delete[] nameBuf;
}
