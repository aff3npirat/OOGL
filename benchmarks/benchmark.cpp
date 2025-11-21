#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <source/shader.h>
#include <stdlib.h>

#include <cassert>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>

#include "config.h"
#include "mandelbrot.h"
#include "utility.h"


BenchmarkStats run_impl(GLFWwindow* window)
{
    unsigned int screenWidth = 780;
    unsigned int screenHeight = 780;
    unsigned int xCubes = 780;
    unsigned int yCubes = 780;

    assert(screenWidth % xCubes == 0);
    assert(screenHeight % yCubes == 0);
    unsigned int numVertex = xCubes * yCubes * 36;

    Buffer buf;
    buf.resize(std::in_place_type<GLfloat>, numVertex * 4);
    BufferView vbo(&buf, 4, 0, 3, GL_FLOAT);
    BufferView cbo(&buf, 4, 3, 1, GL_FLOAT);
    const BufferView* buffers[2] = {&vbo, &cbo};
    Renderer renderer({vbo, cbo});

    GLfloat* vertices = new GLfloat[numVertex * 3];
    GLfloat* colors = new GLfloat[numVertex * 1];

    renderer.begin();
    getVertexData(vertices, colors, xCubes, yCubes, screenWidth, screenHeight);
    for (int i = 0; i < numVertex; i += 36) {
        Mesh mesh(36);
        mesh.addVertexData(&vbo, &vertices[i * 3]);
        mesh.addVertexData(&cbo, &colors[i]);
        renderer.addModel(mesh);
    }
    renderer.end();

    ShaderProgram shader(readFile("../shaders/benchmark.vertexshader").c_str(),
        readFile("../shaders/benchmark.fragmentshader").c_str());

    glClearColor(0.0, 0.0, 0.0, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    clock_t deltaTime = 0.0;
    BenchmarkStats result;
    int counter = 0;
    do {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        clock_t start = clock();
        renderer.end();
        renderer.render();
        clock_t end = clock();

        deltaTime = end - start;
        result.addFramedata(deltaTime / (double)CLOCKS_PER_SEC * UNIT_FACTOR);
        counter++;

        std::cout << counter << " ";

        glfwSwapBuffers(window);
        glfwPollEvents();

    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
             glfwWindowShouldClose(window) == 0 && counter < N_FRAMES);
    std::cout << std::endl;

    delete[] colors;
    delete[] vertices;

    return result;
}


BenchmarkStats run_base(GLFWwindow* window)
{
    unsigned int screenWidth = 780;
    unsigned int screenHeight = 780;
    unsigned int xCubes = 780;
    unsigned int yCubes = 780;

    assert(screenWidth % xCubes == 0);
    assert(screenHeight % yCubes == 0);
    unsigned int numVertex = xCubes * yCubes * 36;

    GLfloat* vertices = new GLfloat[numVertex * 3];
    GLfloat* colors = new GLfloat[numVertex * 1];

    getVertexData(vertices, colors, xCubes, yCubes, screenWidth, screenHeight);

    GLuint vao;
    GLuint vbo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
    glVertexAttribPointer(
        1, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    ShaderProgram shader(readFile("../shaders/benchmark.vertexshader").c_str(),
        readFile("../shaders/benchmark.fragmentshader").c_str());

    glClearColor(0.0, 0.0, 0.0, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    clock_t deltaTime = 0.0;
    BenchmarkStats result;
    int counter = 0;
    do {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        GLfloat* bufData = new GLfloat[numVertex * 4];

        clock_t start = clock();
        for (int i = 0; i < numVertex; i++) {
            bufData[3 + 4 * i] = colors[i];
            for (int j = 0; j < 3; j++) {
                bufData[i * 4 + j] = vertices[i * 3 + j];
            }
        }
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(
            GL_ARRAY_BUFFER, sizeof(GLfloat) * numVertex * 4, (void*)bufData, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glDrawArrays(GL_TRIANGLES, 0, numVertex);
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glBindVertexArray(0);
        clock_t end = clock();

        deltaTime = end - start;
        result.addFramedata(deltaTime / (double)CLOCKS_PER_SEC * UNIT_FACTOR);

        delete[] bufData;
        counter++;

        std::cout << counter << " ";

        glfwSwapBuffers(window);
        glfwPollEvents();
    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
             glfwWindowShouldClose(window) == 0 && counter < N_FRAMES);
    std::cout << std::endl;

    delete[] colors;
    delete[] vertices;

    return result;
}


GLFWwindow* init_gl(std::string title, const int width, const int height)
{
    glfwInit();
    glfwWindowHint(GLFW_SAMPLES, GLFW_DONT_CARE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_FOCUSED, GL_TRUE);
    glfwWindowHint(GLFW_DECORATED, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    glewInit();

    return window;
}


void run_benchmark(std::function<BenchmarkStats(GLFWwindow*)> benchFunc, const char* fpath)
{
    std::cout << "Running benchmark..." << std::endl;

    GLFWwindow* window = init_gl("Benchmark", 780, 780);
    BenchmarkStats result = benchFunc(window);
    writeStats(fpath, result);
}


int main(int argc, char** argv)
{
    if (argc == 1) {    // user invoked
        std::filesystem::path cwd = std::filesystem::current_path();
        std::string exePath = (cwd / "benchmarks.exe").string();
        std::string outFile = (cwd / "tmp.txt").string();

        std::string command = exePath + " base " + outFile;
        std::cout << "Starting baseline..." << std::endl;
        std::system(command.c_str());
        BenchmarkStats baseResults(readStats(outFile.c_str()));
        std::remove(outFile.c_str());

        command = exePath + " impl " + outFile;
        std::cout << "Starting implementation..." << std::endl;
        std::system(command.c_str());
        BenchmarkStats implResults(readStats(outFile.c_str()));
        std::remove(outFile.c_str());

        double avgFac = 0;
        BenchmarkStats::compare(implResults, baseResults, &avgFac);

        std::cout << "Baseline (" UNIT "):" << std::endl;
        std::cout << baseResults.toString(DECIMALS) << std::endl;
        std::cout << "Implementaion (" UNIT "):" << std::endl;
        std::cout << implResults.toString(DECIMALS) << std::endl;
        std::cout << "Factor:" << roundedString(avgFac, DECIMALS) << std::endl;

        return 0;
    }

    std::vector<std::string> args(argv, argv + argc);
    if (args[1] == "base") {    // invoked by main process
        run_benchmark(run_base, args[2].c_str());
    }
    else if (args[1] == "impl") {
        run_benchmark(run_impl, args[2].c_str());
    }

    return 0;
}