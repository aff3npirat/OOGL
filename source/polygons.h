#pragma once

#include <cstring>


template<typename T>
struct Rectangle {
    Rectangle(T x1, T x2, T y1, T y2)
        : x1(x1), x2(x2), y1(y1), y2(y2) {}

    T x1;
    T y1;
    T x2;
    T y2;
};


template<typename T>
void getVertexData(const Rectangle<T>* rect, T* pos, T* uv)
{
    const T positions[12] = {
        rect->x1, rect->y1,
        rect->x1, rect->y2,
        rect->x2, rect->y2,
        rect->x1, rect->y1,
        rect->x2, rect->y2,
        rect->x2, rect->y1
    };
    std::memcpy(pos, positions, 12 * sizeof(T));

    const T uvs[12] = {
        0, 1,
        0, 0,
        1, 0,
        0, 1,
        1, 0,
        1, 1
    };
    std::memcpy(uv, uvs, 12 * sizeof(T));
}