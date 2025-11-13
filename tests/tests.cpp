#include "test_buffer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cstdio>

#include <testsuite.h>


GLFWwindow* init_gl(std::string title, const int width, const int height) {
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


int main() {
    GLFWwindow* window = init_gl("UTS", 780, 780);

    UnitTest::run_tests();

    return 0;
}