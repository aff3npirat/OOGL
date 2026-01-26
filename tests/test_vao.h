#include <testsuite.h>

#include <cstddef>
#include <cstdint>

#include "source/buffer.h"
#include "source/render_context.h"


class TestVertexBuffer : public VertexBuffer {
  public:
    const void* getData() const { return static_cast<void*>(data); }
};

TEST_CASE("VAO::addData - batchwise")
{
    TestVertexBuffer buf(12 * sizeof(int));
    const int* bufData = static_cast<const int*>(buf.getData());
    
    VAO vao(GL_STATIC_DRAW);
    VertexAttribute attrib = {2, GL_FLOAT, GL_FALSE};
    const AttributeBinding* binding = vao.bindBuffer(&attrib, 0, &buf, sizeof(int));
    vao.initialize();

    int data[12] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    vao.addData(binding, data, 3, 0);
    bool passed = true;
    for (int i = 0; i < 6; i++) {
        if (data[i] != bufData[i]) {
            passed = false;
            break;
        }
    }
    
    vao.addData(binding, data, 3, 3);
    for (int i = 3; i < 6; i++) {
        if (data[i] != bufData[i]) {
            passed = false;
            break;
        }
    }

    ASSERT_TRUE(passed);
}