#include "text.h"

#include <GL/glew.h>

#include <stdexcept>
#include <string>
#include <utility>

#include "model.h"


TextRenderer::TextRenderer(const char* fpath)
    : bufferSize(0),
      buffer(),
      vbo(&buffer, 7, 0, 2, GL_FLOAT),
      uvbo(&buffer, 7, 2, 2, GL_FLOAT),
      cbo(&buffer, 7, 4, 3, GL_FLOAT),
      renderer({vbo, uvbo, cbo})
{
    if (FT_Init_FreeType(&library)) {
        throw std::runtime_error("Could not load FreeType2");
    }

    if (FT_New_Face(library, fpath, 0, &face)) {
        throw std::runtime_error("Could not load face " + std::string(fpath));
    }

    FT_Set_Pixel_Sizes(face, 0, 48);
}


TextRenderer::~TextRenderer() {}


void TextRenderer::begin()
{
    bufferSize = 0;
    buffer.resize(std::in_place_type<GLfloat>, 0);
    renderer.begin();
}


void TextRenderer::end()
{
    buffer.resize(std::in_place_type<GLfloat>, bufferSize);
    renderer.end();
}


void TextRenderer::draw(GLfloat x, GLfloat y, GLfloat R, GLfloat G, GLfloat B, const char* text)
{
    Character current;
    for (int i = 0; i < strlen(text); i++) {
        std::map<char, Character>::iterator it = cache.find(text[i]);
        if (it == cache.end()) {
            if (FT_Load_Char(face, text[i], FT_LOAD_RENDER)) {
                throw std::runtime_error("Could not load character " + text[i]);
            }

            current = {generate_texture(text[i], &face->glyph->bitmap), face->glyph->bitmap.width,
                face->glyph->bitmap.rows, face->glyph->bitmap_left, face->glyph->bitmap_top,
                face->glyph->advance.x >> 6};
            cache[text[i]] = current;
        }
        else {
            current = (*it).second;
        }

        GLfloat x1 = x + current.bearingX;
        GLfloat x2 = x1 + current.width;
        GLfloat y1 = y - (current.height - current.bearingY);
        GLfloat y2 = y1 + current.height;

        TexturedMesh mesh(6, current.texture);
        GLfloat vertices[6 * 2] = {// clang-format off
            x1, y1,
            x1, y2,
            x2, y2,
            x1, y1,
            x2, y2,
            x2, y1
        };    // clang-format on
        mesh.addVertexData(&vbo, vertices);

        GLfloat uvs[6 * 2] = {// clang-format off
            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 0.0f,
            1.0f, 1.0f
        };    // clang-format on
        mesh.addVertexData(&uvbo, uvs);

        GLfloat colors[6 * 3] = {
            // clang-format off
            R, G, B,
            R, G, B,
            R, G, B,
            R, G, B,
            R, G, B,
            R, G, B,
        };    // clang-format on
        mesh.addVertexData(&cbo, colors);
        renderer.addModel(mesh);
        bufferSize += 6 * (3 + 2 + 2);

        x += current.advanceX;
    }
}


void TextRenderer::render()
{
    renderer.render();
}


GLuint TextRenderer::generate_texture(char c, FT_Bitmap* bitmap)
{
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    GLuint texture;
    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, bitmap->width, bitmap->rows, 0, GL_RED, GL_UNSIGNED_BYTE,
        (void*)bitmap->buffer);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}