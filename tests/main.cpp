#include <iostream>
#include <cstdio>
#include <string.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "cmake_config.h"
#include "render_context.h"
#include "model.h"
#include "buffer.h"


int init_glfw() {
	return glfwInit();
}


GLFWwindow* init_window(std::string title, const int width, const int height, int focused, int decorated) {
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


GLenum init_glew() {
	glewExperimental = GL_TRUE;
	return glewInit();
}


GLuint load_shaders(const char* vertexShaderPath, const char* fragmentShaderPath) {
	std::string vertexSource;
    std::ifstream VertexShaderStream(vertexShaderPath, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		vertexSource = sstr.str();
	} else {
		printf("Could not open vertex shader '%s'", vertexShaderPath);
		return 0;
	}
	
    std::string fragmentSource;
    std::ifstream FragemntShaderStream(fragmentShaderPath, std::ios::in);
	if (FragemntShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << FragemntShaderStream.rdbuf();
		fragmentSource = sstr.str();
	} else {
		printf("Could not open fragment shader '%s'", fragmentShaderPath);
		return 0;
	}

	printf("Compiling vertex shader\n");
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char * vertexSourcePointer = vertexSource.c_str();
	glShaderSource(vertexShader, 1, &vertexSourcePointer, NULL);
	glCompileShader(vertexShader);

    // check shaders
    GLint status = false;

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint logLen;
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &logLen);
        std::vector<char> log(logLen+1);
        GLsizei written;
        glGetShaderInfoLog(vertexShader, logLen, &written, log.data());
		printf("compile error:\n%s", log.data());
    }

	printf("Compiling fragment shader\n");
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char * fragmentSourcePointer = fragmentSource.c_str();
	glShaderSource(fragmentShader, 1, &fragmentSourcePointer, NULL);
	glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint logLen;
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &logLen);
        std::vector<char>log(logLen+1);
        GLsizei written;
        glGetShaderInfoLog(fragmentShader, logLen, &written, log.data());
		printf("compile error:\n%s", log.data());
    }

	printf("Linking shaders\n");
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint logLen;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logLen);
        std::vector< char >log(logLen+1);
        GLsizei written;
        glGetProgramInfoLog(shaderProgram, logLen, &written, log.data());
		printf("link error:\n%s", log.data());
    }

	return shaderProgram;
}

GLuint loadBMP_custom(const char * imagepath){

	printf("Reading image %s\n", imagepath);

	// Data read from the header of the BMP file
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	unsigned int width, height;
	// Actual RGB data
	unsigned char * data;

	// Open the file
	FILE * file = fopen(imagepath,"rb");
	if (!file){
		printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath);
		getchar();
		return 0;
	}

	// Read the header, i.e. the 54 first bytes

	// If less than 54 bytes are read, problem
	if ( fread(header, 1, 54, file)!=54 ){ 
		printf("Not a correct BMP file\n");
		fclose(file);
		return 0;
	}
	// A BMP files always begins with "BM"
	if ( header[0]!='B' || header[1]!='M' ){
		printf("Not a correct BMP file\n");
		fclose(file);
		return 0;
	}
	// Make sure this is a 24bpp file
	if ( *(int*)&(header[0x1E])!=0  )         {printf("Not a correct BMP file\n");    fclose(file); return 0;}
	if ( *(int*)&(header[0x1C])!=24 )         {printf("Not a correct BMP file\n");    fclose(file); return 0;}

	// Read the information about the image
	dataPos    = *(int*)&(header[0x0A]);
	imageSize  = *(int*)&(header[0x22]);
	width      = *(int*)&(header[0x12]);
	height     = *(int*)&(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if (imageSize==0)    imageSize=width*height*3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos==0)      dataPos=54; // The BMP header is done that way

	// Create a buffer
	data = new unsigned char [imageSize];

	// Read the actual data from the file into the buffer
	fread(data,1,imageSize,file);

	// Everything is in memory now, the file can be closed.
	fclose (file);

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);
	
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

	// OpenGL has now copied the data. Free our own version
	delete [] data;

	// Poor filtering, or ...
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 

	// ... nice trilinear filtering ...
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	// ... which requires mipmaps. Generate them automatically.
	glGenerateMipmap(GL_TEXTURE_2D);

	// Return the ID of the texture we just created
	return textureID;
}


int main() {
    init_glfw();
    GLFWwindow* window = init_window(
        "OGLEngine " + std::to_string(PROJECT_VERSION_MAJOR) + "." + std::to_string(PROJECT_VERSION_MINOR),
        780, 780, GL_TRUE, GL_TRUE
    );
    glfwMakeContextCurrent(window);

    init_glew();
    glfwSwapInterval(0);

    GLuint textureShader = load_shaders(
        "../shaders/vertex.vertexshader",
        "../shaders/fragment.fragmentshader"
    );
    glUseProgram(textureShader);
    GLuint textureSampler = glGetUniformLocation(textureShader, "textureSampler");

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(
        glm::vec3(3.0, 3.0, -8.0),
        glm::vec3(0.0, 0.0, 0.0),
        glm::vec3(0.0, 1.0, 0.0)
    );
    glm::mat4 VP = projection * view;
    GLuint cameraMatID = glGetUniformLocation(textureShader, "VP");
    glUniformMatrix4fv(cameraMatID, 1, GL_FALSE, &VP[0][0]);

    GLuint texture = loadBMP_custom("../resources/uvtemplate.bmp");

	Buffer buffer;
	buffer.init(std::in_place_type<GLfloat>, 180);

	BufferView vbo(&buffer, 5, 0, 3, GL_FLOAT);
	BufferView uvbo(&buffer, 5, 3, 2, GL_FLOAT);
	
	GLfloat cubeVertices[36*3] =
    {
        -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f, 
        1.0f, 1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        -1.0f,-1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f,-1.0f,
        1.0f,-1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f,-1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f,-1.0f, 1.0f
    };

	GLfloat cubeUvs[36*2] = {
        0.000059f, 1.0f-0.000004f,
        0.000103f, 1.0f-0.336048f,
        0.335973f, 1.0f-0.335903f,
        1.000023f, 1.0f-0.000013f,
        0.667979f, 1.0f-0.335851f,
        0.999958f, 1.0f-0.336064f,
        0.667979f, 1.0f-0.335851f,
        0.336024f, 1.0f-0.671877f,
        0.667969f, 1.0f-0.671889f,
        1.000023f, 1.0f-0.000013f,
        0.668104f, 1.0f-0.000013f,
        0.667979f, 1.0f-0.335851f,
        0.000059f, 1.0f-0.000004f,
        0.335973f, 1.0f-0.335903f,
        0.336098f, 1.0f-0.000071f,
        0.667979f, 1.0f-0.335851f,
        0.335973f, 1.0f-0.335903f,
        0.336024f, 1.0f-0.671877f,
        1.000004f, 1.0f-0.671847f,
        0.999958f, 1.0f-0.336064f,
        0.667979f, 1.0f-0.335851f,
        0.668104f, 1.0f-0.000013f,
        0.335973f, 1.0f-0.335903f,
        0.667979f, 1.0f-0.335851f,
        0.335973f, 1.0f-0.335903f,
        0.668104f, 1.0f-0.000013f,
        0.336098f, 1.0f-0.000071f,
        0.000103f, 1.0f-0.336048f,
        0.000004f, 1.0f-0.671870f,
        0.336024f, 1.0f-0.671877f,
        0.000103f, 1.0f-0.336048f,
        0.336024f, 1.0f-0.671877f,
        0.335973f, 1.0f-0.335903f,
        0.667969f, 1.0f-0.671889f,
        1.000004f, 1.0f-0.671847f,
        0.667979f, 1.0f-0.335851f
    };
	BufferView attribs[2] = {vbo, uvbo};

	Textured3D cube(&vbo, &cubeVertices[0], &uvbo, &cubeUvs[0], texture, 36);
	unsigned int shaderAttribs[2] = {0, 1};

	Render<Textured3D> context(&attribs[0], &shaderAttribs[0], 2);
	context.addModel(&cube);

	glClearColor(0.0, 0.0, 0.0, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    do {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(textureShader);
        glUniformMatrix4fv(cameraMatID, 1, GL_FALSE, &VP[0][0]);
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(textureSampler, 0);

        context.render();

        glfwSwapBuffers(window);
        glfwPollEvents();

    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

    return 0;
}