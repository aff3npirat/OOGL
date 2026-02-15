#include "text.h"

#include <GL/glew.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <cstring>
#include <utility>
#include <stdexcept>


TextRender::FaceCache TextRender::_cache = TextRender::FaceCache();


GLuint generateTexture(char c, FT_Bitmap* bitmap)
{
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    GLuint texture;
    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D,
        0,
        GL_RED,
        bitmap->width,
        bitmap->rows,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        (void*)bitmap->buffer);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}


TextRender::TextRender(
    const char* fpath, signed long idx)
{
    if (FT_Init_FreeType(&library)) {
        throw std::runtime_error("Could not load FreeType2");
    }

    if (FT_New_Face(library, fpath, idx, &face)) {
        throw std::runtime_error("Could not load face " + std::string(fpath));
    }

    FT_Set_Pixel_Sizes(face, 0, 48);

    FaceID faceId = std::make_pair(fpath, idx);
    if (!_cache.contains(faceId)) {
        _cache[faceId] = CharCache();
    }

    cache = &_cache[faceId];
}


void TextRender::add(const char* text, float x1, float y1, float x2, float y2)
{
    unsigned int totalWidth = 0;
    unsigned int maxHeight = 0;
    int maxBearingY = 0;
    unsigned int n = strlen(text);

    for (int i = 0; i < n; i++) {
        char c = text[i];
        Character current;
        if (cache->contains(c)) {
            current = (*cache)[0];
        }
        else if (!FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            current = {generateTexture(text[i], &face->glyph->bitmap), face->glyph->bitmap.width,
                face->glyph->bitmap.rows, face->glyph->bitmap_left, face->glyph->bitmap_top,
                face->glyph->advance.x >> 6};

            (*cache)[c] = current;
        }
        else {
            throw std::runtime_error("Could not load char " + c);
        }

        totalWidth += current.advanceX;
        if (current.height > maxHeight) {
            maxHeight = current.height;
            maxBearingY = current.bearingY;
        }
    }

    float boxWidth = x2 - x1;
    float boxHeight = y2 - y1;
    float cursorY = y2 - (boxHeight * maxBearingY / maxHeight);

    for (int i = 0; i < n; i++) {
        Character* current = &(*cache)[text[i]];
        float relWidth = boxWidth * current->width / totalWidth;
        float relHeight = boxHeight * current->height / maxHeight;
        float relBearingX = relWidth * current->bearingX / current->width;
        float relBearingY = relHeight * current->bearingY / current->height;
        float relNegBearingY = relHeight - relBearingY;

        glyphs[current->texture].emplace_back(
            x1 + relBearingX,
            x1 + relWidth + relBearingX,
            cursorY - relNegBearingY,
            cursorY + relBearingY
        );

        x1 += boxWidth * current->advanceX / totalWidth;
    }
}


unsigned int TextRender::draw(const AttributeBinding* position,
    const AttributeBinding* uv,
    GLuint* textures,
    unsigned int* offsets)
{
    std::size_t idx = 0;
    unsigned int offset = position->buffer->size() / position->stride;
    float data[24];
    for (glyph_map_t::value_type v : glyphs) {
        textures[idx] = v.first;
        offsets[idx++] = offset;

        for (glyph_map_t::mapped_type::value_type rect : v.second) {
            getVertexData(&rect, data, &(data[12]));

            position->buffer->add(
                static_cast<void*>(data),
                sizeof(float) * 12,
                position->attribute->size * position->valSize,
                position->stride,
                offset * position->stride + position->offset
            );

            uv->buffer->add(
                static_cast<void*>(&(data[12])),
                sizeof(float) * 12,
                uv->attribute->size * uv->valSize,
                uv->stride,
                offset * uv->stride + uv->offset
            );

            offset += 6;
        }
    }

    return idx + 1;
}