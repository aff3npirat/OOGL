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


TEST_CASE("BufferView::insert (ZERO_STRIDE)")
{
    Buffer buf;
    setupIntBuffer(10, buf);
    int* data = new int[10];
    for (int i = 0; i < 10; i++) {
        data[i] = i;
    }

    BufferView view(&buf, 0, 0, 1, GL_INT);
    view.insert(data, 10, 0);

    int* bufData = static_cast<int*>(buf.data());
    for (int i = 0; i < 10; i++) {
        ASSERT_TRUE(bufData[i] == i);
    }
}


TEST_CASE("BufferView::insert (STRIDE=VSIZE)")
{
    Buffer buf;
    setupIntBuffer(10, buf);
    int* data = new int[10];
    for (int i = 0; i < 10; i++) {
        data[i] = i;
    }

    BufferView view(&buf, 2, 0, 2, GL_INT);
    view.insert(data, 10, 0);

    int* bufData = static_cast<int*>(buf.data());
    for (int i = 0; i < 10; i++) {
        ASSERT_TRUE(bufData[i] == i);
    }
}


TEST_CASE("BufferView::insert (NORMAL CASE)")
{
    Buffer buf;
    setupIntBuffer(20, buf);
    int* data = new int[10];
    for (int i = 0; i < 10; i++) {
        data[i] = i;
    }

    BufferView view(&buf, 4, 0, 2, GL_INT);
    view.insert(data, 10, 0);

    int count = 0;
    int* bufData = static_cast<int*>(buf.data());
    for (int i = 0; i < 20; i++) {
        if (i % 4 == 0 || (i - 1) % 4 == 0) {
            ASSERT_TRUE(bufData[i] == count);
            count++;
        }
        else {
            ASSERT_TRUE(bufData[i] == 0);
        }
    }
}


TEST_CASE("BufferView::insert (OFFSET)")
{
    Buffer buf;
    setupIntBuffer(10, buf);
    int* data = new int[1];
    *data = 10;

    BufferView view(&buf, 0, 9, 1, GL_INT);
    view.insert(data, 1, 0);

    int* bufData = static_cast<int*>(buf.data());
    ASSERT_TRUE(bufData[9] == 10);
    for (int i = 0; i < 9; i++) {
        ASSERT_TRUE(bufData[i] == 0);
    }
}


TEST_CASE("BufferView::insert (TEMPOFFSET)")
{
    Buffer buf;
    setupIntBuffer(10, buf);
    int* data = new int[1];
    *data = 10;
    
    BufferView view(&buf, 0, 5, 1, GL_INT);
    view.insert(data, 1, 4);
    
    int* bufData = static_cast<int*>(buf.data());
    ASSERT_TRUE(bufData[9] == 10);
    for (int i = 0; i < 9; i++) {
        ASSERT_TRUE(bufData[i] == 0);
    }
}