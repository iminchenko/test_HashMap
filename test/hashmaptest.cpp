#include <gtest/gtest.h>
#include <thread>

#include "hashmap.hpp"

constexpr size_t N = 10000;

TEST(insertion, insertOneFunc) {
    HashMap<int> h;

    h.insert(1, 1);

    ASSERT_EQ(h[1], 1);
}

TEST(insertion, insertOneOperator) {
    HashMap<int> h;

    h[1] = 1;

    ASSERT_EQ(h[1], 1);
}

TEST(insertion, insertNFunc) {
    HashMap<int> h;

    for (size_t i = 0; i < N; ++i) {
        h.insert(i, i);
    }

    ASSERT_EQ(h.size(), N);

    for (size_t i = 0; i < N; ++i) {
        ASSERT_EQ(h[i], i) << i;
    }
}

TEST(insertion, insertNReverseIndexFunc) {
    HashMap<int> h;

    for (size_t i = 0; i < N; ++i) {
        h.insert(N - i, i);
    }

    ASSERT_EQ(h.size(), N);

    for (size_t i = 0; i < N; ++i) {
        ASSERT_EQ(h[N - i], i) << i;
    }
}

TEST(insertion, insertNOperator) {
    HashMap<int> h;

    for (size_t i = 0; i < N; ++i) {
        h[i] = i;
    }

    ASSERT_EQ(h.size(), N);

    for (size_t i = 0; i < N; ++i) {
        ASSERT_EQ(h[i], i) << i;
    }
}

TEST(insertion, insertNMultythreadFunc) {
    HashMap<int> h;

    std::thread t1([&h](){
        for (size_t i = 0; i < N; ++i) {
            h.insert(i, i);
        }
    });
    std::thread t2([&h](){
        for (size_t i = 0; i < N; ++i) {
            h.insert(i + N, i + N);
        }
    });

    t1.join();
    t2.join();

    ASSERT_EQ(h.size(), 2 * N);

    for (size_t i = 0; i < 2 * N; ++i) {
        ASSERT_EQ(h[i], i) << i;
    }
}

TEST(insertion, insertNMultythreadOperator) {
    HashMap<int> h;

    std::thread t1([&h](){
        for (size_t i = 0; i < N; ++i) {
            h[i] = i;
        }
    });
    std::thread t2([&h](){
        for (size_t i = 0; i < N; ++i) {
            h[i + N] = i + N;
        }
    });

    t1.join();
    t2.join();

    ASSERT_EQ(h.size(), 2 * N);

    for (size_t i = 0; i < 2 * N; ++i) {
        ASSERT_EQ(h[i], i) << i;
    }
}
