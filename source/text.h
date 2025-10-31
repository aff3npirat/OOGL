#pragma once

#include <GL/glew.h>
#include <ft2build.h>

#include <map>
#include <vector>
#include FT_FREETYPE_H

#include "render_context.h"


struct Glyph {
    Glyph(GLuint texture, GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat R, GLfloat G,
        GLfloat B);

    GLuint texture;
    GLfloat vertices[12];
    GLfloat colors[3];
};

struct Character {
    GLuint texture;
    unsigned int width, height;
    int bearingX, bearingY;
    long int advanceX;
};

class TextRenderer {
  public:
    TextRenderer(const char* fpath, Buffer* buffer);
    ~TextRenderer();

    void draw(GLfloat x, GLfloat y, GLfloat R, GLfloat G, GLfloat B, const char* text);
    void render();

  private:
    GLuint generate_texture(char c, FT_Bitmap* bitmap);

    Renderer<TexturedMesh> renderer;
    BufferView vbo;
    BufferView cbo;
    FT_Library library;
    FT_Face face;
    std::map<char, Character> cache;
};