#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <string.h>

#include <cstdio>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <sstream>
#include <vector>

#include "cmake_config.h"
#include "source/buffer.h"
#include "source/render_context.h"
#include "source/shader.h"
#include "source/text.h"


int init_glfw()
{
    return glfwInit();
}


GLFWwindow* init_window(
    std::string title, const int width, const int height, int focused, int decorated)
{
    glfwWindowHint(GLFW_SAMPLES, GLFW_DONT_CARE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_FOCUSED, focused);
    glfwWindowHint(GLFW_DECORATED, decorated);

    GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    return window;
}


GLenum init_glew()
{
    glewExperimental = GL_TRUE;
    return glewInit();
}


std::string readFile(const char* fpath)
{
    std::string content = "";
    std::ifstream Stream(fpath, std::ios::in);
    if (Stream.is_open()) {
        std::stringstream sstr;
        sstr << Stream.rdbuf();
        content = sstr.str();
    }
    else {
        printf("Could not open file '%s'", fpath);
    }

    return content;
}


GLuint loadBMP_custom(const char* imagepath)
{
    printf("Reading image %s\n", imagepath);

    // Data read from the header of the BMP file
    unsigned char header[54];
    unsigned int dataPos;
    unsigned int imageSize;
    unsigned int width, height;
    // Actual RGB data
    unsigned char* data;

    // Open the file
    FILE* file = fopen(imagepath, "rb");
    if (!file) {
        printf(
            "%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ "
            "!\n",
            imagepath);
        getchar();
        return 0;
    }

    // Read the header, i.e. the 54 first bytes

    // If less than 54 bytes are read, problem
    if (fread(header, 1, 54, file) != 54) {
        printf("Not a correct BMP file\n");
        fclose(file);
        return 0;
    }
    // A BMP files always begins with "BM"
    if (header[0] != 'B' || header[1] != 'M') {
        printf("Not a correct BMP file\n");
        fclose(file);
        return 0;
    }
    // Make sure this is a 24bpp file
    if (*(int*)&(header[0x1E]) != 0) {
        printf("Not a correct BMP file\n");
        fclose(file);
        return 0;
    }
    if (*(int*)&(header[0x1C]) != 24) {
        printf("Not a correct BMP file\n");
        fclose(file);
        return 0;
    }

    // Read the information about the image
    dataPos = *(int*)&(header[0x0A]);
    imageSize = *(int*)&(header[0x22]);
    width = *(int*)&(header[0x12]);
    height = *(int*)&(header[0x16]);

    // Some BMP files are misformatted, guess missing information
    if (imageSize == 0)
        imageSize = width * height * 3;    // 3 : one byte for each Red, Green and Blue component
    if (dataPos == 0) dataPos = 54;        // The BMP header is done that way

    // Create a buffer
    data = new unsigned char[imageSize];

    // Read the actual data from the file into the buffer
    fread(data, 1, imageSize, file);

    // Everything is in memory now, the file can be closed.
    fclose(file);

    // Create one OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Give the image to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

    // OpenGL has now copied the data. Free our own version
    delete[] data;

    // Poor filtering, or ...
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // ... nice trilinear filtering ...
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // ... which requires mipmaps. Generate them automatically.
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    // Return the ID of the texture we just created
    return textureID;
}


int main()
{
    init_glfw();
    GLFWwindow* window = init_window("OGLEngine " + std::to_string(PROJECT_VERSION_MAJOR) + "." +
                                         std::to_string(PROJECT_VERSION_MINOR),
        780, 780, GL_TRUE, GL_TRUE);
    glfwMakeContextCurrent(window);

    init_glew();
    glfwSwapInterval(0);

    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));

    std::string vertexSource = readFile("../shaders/vertex.vertexshader");
    std::string fragmentSource = readFile("../shaders/fragment.fragmentshader");

    ShaderProgram shader(vertexSource.c_str(), fragmentSource.c_str());
    shader.registerGLSetting([]() { glActiveTexture(GL_TEXTURE0); });

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
    glm::mat4 view =
        glm::lookAt(glm::vec3(3.0, 3.0, -8.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 VP = projection * view;
    shader.bindUniform("VP", GL_FALSE, &VP[0][0]);

    const GLint textureIdx = 0;
    shader.bindUniform("textureSampler", &textureIdx);

    GLuint texture = loadBMP_custom("../resources/uvtemplate.bmp");

    VertexBuffer buffer(180 * sizeof(GLfloat));

    VertexAttribute vertexFormat {3, GL_FLOAT, GL_FALSE};
    VertexAttribute uvFormat {2, GL_FLOAT, GL_FALSE};

    std::vector<const AttributeBinding*> bindings;
    bindings.resize(shader.getNumAttribs());

    VAO vao(GL_STATIC_DRAW);
    bindings[shader.getAttribIndex("vertexPosition_modelspace")] = vao.bindBuffer(&vertexFormat, shader.getAttribIndex("vertexPosition_modelspace"), &buffer, sizeof(GLfloat));
    bindings[shader.getAttribIndex("uv")] = vao.bindBuffer(&uvFormat, shader.getAttribIndex("uv"), &buffer, sizeof(GLfloat));
    vao.initialize();

    GLfloat cubeVertices[36 * 3] = {-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f};

    GLfloat cubeUvs[36 * 2] = {0.000059f, 1.0f - 0.000004f, 0.000103f, 1.0f - 0.336048f, 0.335973f,
        1.0f - 0.335903f, 1.000023f, 1.0f - 0.000013f, 0.667979f, 1.0f - 0.335851f, 0.999958f,
        1.0f - 0.336064f, 0.667979f, 1.0f - 0.335851f, 0.336024f, 1.0f - 0.671877f, 0.667969f,
        1.0f - 0.671889f, 1.000023f, 1.0f - 0.000013f, 0.668104f, 1.0f - 0.000013f, 0.667979f,
        1.0f - 0.335851f, 0.000059f, 1.0f - 0.000004f, 0.335973f, 1.0f - 0.335903f, 0.336098f,
        1.0f - 0.000071f, 0.667979f, 1.0f - 0.335851f, 0.335973f, 1.0f - 0.335903f, 0.336024f,
        1.0f - 0.671877f, 1.000004f, 1.0f - 0.671847f, 0.999958f, 1.0f - 0.336064f, 0.667979f,
        1.0f - 0.335851f, 0.668104f, 1.0f - 0.000013f, 0.335973f, 1.0f - 0.335903f, 0.667979f,
        1.0f - 0.335851f, 0.335973f, 1.0f - 0.335903f, 0.668104f, 1.0f - 0.000013f, 0.336098f,
        1.0f - 0.000071f, 0.000103f, 1.0f - 0.336048f, 0.000004f, 1.0f - 0.671870f, 0.336024f,
        1.0f - 0.671877f, 0.000103f, 1.0f - 0.336048f, 0.336024f, 1.0f - 0.671877f, 0.335973f,
        1.0f - 0.335903f, 0.667969f, 1.0f - 0.671889f, 1.000004f, 1.0f - 0.671847f, 0.667979f,
        1.0f - 0.335851f};

    vao.begin();
    vao.addData(bindings[shader.getAttribIndex("vertexPosition_modelspace")], cubeVertices, 36);
    vao.addData(bindings[shader.getAttribIndex("uv")], cubeUvs, 36);
    vao.end();

    vertexSource = readFile("../shaders/text.vertexshader");
    fragmentSource = readFile("../shaders/text.fragmentshader");
    ShaderProgram textShader(vertexSource.c_str(), fragmentSource.c_str());

    VertexBuffer textBuf(1);
    VertexAttribute textPosFmt {2, GL_FLOAT, GL_FALSE};
    VertexAttribute textUVFmt {2, GL_FLOAT, GL_FALSE};
    VAO textVAO(GL_STATIC_DRAW);
    const AttributeBinding* textPos = textVAO.bindBuffer(&textPosFmt, 0, &textBuf, sizeof(float));
    const AttributeBinding* textUV = textVAO.bindBuffer(&textUVFmt, 1, &textBuf, sizeof(float));
    textVAO.initialize();

    // glm::mat4 textProjection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
    glm::mat4 textProjection = glm::mat4(1.0f);  // Identity
    textShader.bindUniform("P", GL_FALSE, &textProjection[0][0]);
    textShader.bindUniform("textureSampler", &textureIdx);
    textShader.registerGLSetting([]() {
        glActiveTexture(GL_TEXTURE0);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    });

    TextRender textContext("../resources/fonts/ARIALMT.ttf", 0, 780, 780);
    textContext.add("HE", 0.7, 0.45, 0.9, 0.55);

    textVAO.begin();
    GLuint* charTextures = new GLuint[textContext.getNumTextures()];
    unsigned int* offsets = new unsigned int[textContext.getNumTextures()];
    unsigned int numTextures = textContext.draw(
        textPos,
        textUV,
        charTextures,
        offsets
    );
    textVAO.end();

    glClearColor(0.0, 0.0, 0.0, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    do {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDisable(GL_BLEND);
        shader.use();
        glBindTexture(GL_TEXTURE_2D, texture);        
        vao.render(0, 180);

        textShader.use();
        for (int i = 0; i < numTextures - 1; i++) {
            glBindTexture(GL_TEXTURE_2D, charTextures[i]);
            textVAO.render(offsets[i], offsets[i + 1] - offsets[i]);
        }
        glBindTexture(GL_TEXTURE_2D, charTextures[numTextures - 1]);
        textVAO.render(offsets[numTextures - 1], vao.getNumVertex() - offsets[numTextures - 1]);

        glfwSwapBuffers(window);
        glfwPollEvents();

    } while (
        glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

    return 0;
}