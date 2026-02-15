#pragma once

#include <GL/glew.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <map>
#include <string>
#include <utility>

#include "render_context.h"
#include "polygons.h"


struct Character {
    GLuint texture;
    unsigned int width, height;
    int bearingX, bearingY;
    long int advanceX;
};


GLuint generateTexture(char c, FT_Bitmap* bitmap);


class TextRender {
  public:
    TextRender(const char* fpath, signed long idx);

    /// @brief Stores text to be rendered.
    /// @param text Text to render.
    /// @param x1, y1, x2, y2 Text will be placed in bounds (x1, y1) <->
    /// (x2, y2).
    void add(const char* text, float x1, float y1, float x2, float y2);

    /// @brief Clears all collected text.
    void clear();

    /// @brief Inserts vertex data of stored text into buffers.
    /// @param position, uv, color Specify buffers/layout in which data is
    /// inserted.
    /// @param textures, offsets Returns id and vertex offset (from buffer
    /// beginning) for each used texture.
    /// @return Number of vertices written to `textures` and `offsets`.
    unsigned int draw(const AttributeBinding* position,
        const AttributeBinding* uv,
        GLuint* textures,
        unsigned int* offsets);

    /// @brief Returns number of different textures currently used.
    std::size_t getNumTextures() const { return glyphs.size(); }

  private:
    using FaceID = std::pair<std::string, FT_Long>;
    using CharCache = std::map<char, Character>;
    using FaceCache = std::map<FaceID, CharCache>;

    static FaceCache _cache;

    FT_Library library;
    FT_Face face;
    CharCache* cache;
    using glyph_map_t = std::map<GLuint, std::vector<Rectangle<float>>>;
    glyph_map_t glyphs;
};