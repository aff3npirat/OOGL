#include <testsuite.h>

#include <cstdint>
#include <memory>
#include <utility>

#include "source/buffer.h"


void setupIntBuffer(unsigned int size, Buffer& buf) {
    buf.resize(std::in_place_type<int>, size);
    int* bufData = static_cast<int*>(buf.data());
    for (int i = 0; i < size; i++) {
        bufData[i] = 0;
    }
}


TEST_CASE("Buffer::resize")
{
    Buffer buf;
    setupIntBuffer(50, buf);

    unsigned int expected = sizeof(int) * 50;
    ASSERT_TRUE(expected == buf.getSize() * buf.getByteSize());
    ASSERT_TRUE(buf.getId() != 0);
}