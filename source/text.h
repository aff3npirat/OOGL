#pragma once

#include <GL/glew.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <map>
#include <vector>
#include <cstdlib>

#include "render_context.h"


struct Character {
    GLuint texture;
    unsigned int width, height;
    int bearingX, bearingY;
    long int advanceX;
};

class TextRenderer {
  public:
    TextRenderer(const char* fpath);
    ~TextRenderer();

    void begin();
    void end();
    void draw(GLfloat x, GLfloat y, GLfloat R, GLfloat G, GLfloat B, const char* text);
    void render();

  private:
    GLuint generate_texture(char c, FT_Bitmap* bitmap);

    std::size_t bufferSize;
    Buffer buffer;
    BufferView vbo;
    BufferView uvbo;
    BufferView cbo;
    TextureRenderer renderer;
    FT_Library library;
    FT_Face face;
    std::map<char, Character> cache;
};