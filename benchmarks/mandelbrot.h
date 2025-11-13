#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>

#include "buffer.h"
#include "model.h"
#include "render_context.h"


class MandelBrot {
  public:
    MandelBrot(double min_x, double max_x, double min_y, double max_y, unsigned int width,
        unsigned int height, unsigned int stop_d)
        : min_x(min_x),
          max_x(max_x),
          min_y(min_y),
          max_y(max_y),
          width(width),
          height(height),
          stop_d(stop_d)
    {
        this->pixel_data = new float[width * height];
    }
    ~MandelBrot() { delete[] pixel_data; }

    float* calculate()
    {
        for (int y_ = 0; y_ < height; y_++) {
            for (int x_ = 0; x_ < width; x_++) {
                double x = ((double)x_ / width) * (max_x - min_x) + min_x;
                double y = ((double)y_ / height) * (max_y - min_y) + min_y;

                double x_n = x;
                double y_n = y;
                int d;
                for (d = 0; d < stop_d; d++) {
                    if (sqrt(x_n * x_n + y_n * y_n) > 2) {
                        break;
                    }

                    double x_new = x_n * x_n - y_n * y_n + x;
                    double y_new = 2 * x_n * y_n + y;

                    x_n = x_new;
                    y_n = y_new;
                }

                float* pixPtr = (pixel_data + y_ * width + x_);
                if (d < stop_d) {
                    *pixPtr = log((double)(d % (stop_d - 1)) + 1) / log(stop_d);
                }
                else {
                    *pixPtr = 0x00;
                }
            }
        }

        return pixel_data;
    }

  private:
    double min_x;
    double max_x;
    double min_y;
    double max_y;
    unsigned int width;
    unsigned int height;
    unsigned int stop_d;
    float* pixel_data;
};


void getCube(
    GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x8, GLfloat y8, GLfloat z8, GLfloat* vertices)
{
    GLfloat y2 = y1;
    GLfloat y5 = y1;
    GLfloat y6 = y1;
    GLfloat y3 = y8;
    GLfloat y4 = y8;
    GLfloat y7 = y8;

    GLfloat x3 = x1;
    GLfloat x5 = x1;
    GLfloat x7 = x1;
    GLfloat x2 = x8;
    GLfloat x4 = x8;
    GLfloat x6 = x8;

    GLfloat z2 = z1;
    GLfloat z3 = z1;
    GLfloat z4 = z1;
    GLfloat z5 = z8;
    GLfloat z6 = z8;
    GLfloat z7 = z8;

    // clang-format off
    GLfloat tmp[36 * 3] = {
        x1, y1, z1, x2, y2, z2, x3, y3, z3, x2, y2, z2, x3, y3, z3, x4, y4, z4,  // Bottom
        x2, y2, z2, x6, y6, z6, x4, y4, z4, x6, y6, z6, x4, y4, z4, x8, y8, z8,  // Right
        x1, y1, z1, x3, y3, z3, x5, y5, z5, x3, y3, z3, x5, y5, z5, x7, y7, z7,  // Left
        x3, y3, z3, x4, y4, z4, x7, y7, z7, x4, y4, z4, x7, y7, z7, x8, y8, z8,  // Front
        x1, y1, z1, x2, y2, z2, x5, y5, z5, x2, y2, z2, x5, y5, z5, x6, y6, z6,  // Back
        x5, y5, z5, x6, y6, z6, x7, y7, z7, x6, y6, z6, x7, y7, z7, x8, y8, z8   // Top
    };
    // clang-format on

    std::copy(tmp, tmp + 36 * 3, vertices);
}


void getVertexData(GLfloat* vertices, GLfloat* colors, unsigned int xCubes, unsigned int yCubes,
    unsigned int screenWidth, unsigned int screenHeight)
{
    unsigned int cubeWidth = screenWidth / xCubes;
    unsigned int cubeHeight = screenHeight / yCubes;

    MandelBrot mandel(-2.0, 1.0, -1.0, 1.0, xCubes, yCubes, 255);
    float* pixel_data = mandel.calculate();

    for (int y = 0; y < yCubes; y++) {
        for (int x = 0; x < xCubes; x++) {
            float pixVal = *(pixel_data + y * xCubes + x);

            GLfloat* cubeColor = colors + (y * xCubes + x) * 36;
            for (int i = 0; i < 36; i++) {
                cubeColor[i] = (GLfloat)pixVal;
            }

            double xFac = (double)cubeWidth / screenWidth;
            double yFac = (double)cubeHeight / screenHeight;
            GLfloat x1 = x * xFac * 2.0f - 1.0f;
            GLfloat y1 = y * yFac * 2.0f - 1.0f;
            GLfloat z1 = 0;
            GLfloat x8 = (x + 1) * xFac * 2.0f - 1.0f;
            GLfloat y8 = (y + 1) * yFac * 2.0f - 1.0f;
            GLfloat z8 = 1;

            GLfloat* cubeVertices = vertices + (y * yCubes + x) * 36 * 3;
            getCube(x1, y1, z1, x8, y8, z8, cubeVertices);
        }
    }
}