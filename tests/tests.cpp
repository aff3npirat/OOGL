#include "test_buffer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cstdio>

#include <testsuite.h>



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


int main() {
    init_glfw();
    GLFWwindow* window = init_window("UTS", 780, 780, GL_TRUE, GL_TRUE);
    glfwMakeContextCurrent(window);
    init_glew();

    UnitTest::run_tests();

    return 0;
}