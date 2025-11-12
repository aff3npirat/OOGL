#include <testsuite.h>

#include <cstdint>
#include <memory>
#include <utility>

#include "source/buffer.h"


void setupIntBuffer(unsigned int size, Buffer& buf) {
    buf.init(std::in_place_type<int>, size);
    int* bufData = static_cast<int*>(buf.data());
    for (int i = 0; i < size; i++) {
        bufData[i] = 0;
    }
}


TEST_CASE("Buffer::init")
{
    Buffer buf;
    setupIntBuffer(50, buf);

    unsigned int expected = sizeof(int) * 50;
    ASSERT_TRUE(expected == buf.size() * buf.byteSize());
    ASSERT_TRUE(buf.id() != 0);
}