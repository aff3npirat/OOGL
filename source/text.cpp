#include "text.h"

#include <GL/glew.h>

#include <stdexcept>
#include <string>


TextRenderer::TextRenderer(const char* fpath, Buffer* buffer)
    : vbo(buffer, 5, 0, 2, GL_FLOAT), cbo(buffer, 5, 2, 3, GL_FLOAT), renderer({vbo, cbo})
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

        GLfloat vertices_[12] = {// clang-format off
            x1, y1,
            x1, y2,
            x2, y2,
            x1, y1,
            x2, y2,
            x2, y1
        };    // clang-format on
        VData vertices(vertices_, 12, 2);
        GLfloat colors_[3] = {R, G, B};
        VData colors(colors_, 3, 3);    // TODO colors must be provided per vertex
        // TODO add uv coordinates as attrib

        const VData attribs[2] = {vertices, colors};
        const BufferView buffers[2] = {vbo, cbo};
        TexturedMesh mesh(buffers, attribs, 2, 6, current.texture);
        renderer.addModel(&mesh);

        x += current.advanceX;
    }
}


void TextRenderer::render() {}


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

    return texture;
}


Glyph::Glyph(
    GLuint texture, GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat R, GLfloat G, GLfloat B)
    : texture(texture), vertices{x1, y1, x1, y2, x2, y2, x1, y1, x2, y2, x2, y1}, colors{R, G, B}
{
}