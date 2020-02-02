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

TEST(insertion, insertKeyDuplicate) {
    HashMap<int> h;

    h.insert(1, 1);

    ASSERT_EQ(h[1], 1);

    h.insert(1, 2);

    ASSERT_EQ(h[1], 2);
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

TEST(access, exception) {
    const HashMap<int> h;

    ASSERT_THROW(h[1], std::logic_error);
}

TEST(contain, correctOne) {
    HashMap<int> h;

    h.insert(1, 1);

    ASSERT_TRUE(h.contains(1));
}

TEST(contain, incorrectOne) {
    HashMap<int> h;

    ASSERT_FALSE(h.contains(1));

    h.insert(1, 1);

    ASSERT_FALSE(h.contains(2));
}

TEST(contain, N) {
    HashMap<int> h;

    for (size_t i = 0; i < N; ++i) {
        ASSERT_FALSE(h.contains(i));

        h.insert(i, i);

        ASSERT_TRUE(h.contains(i));
    }
}

TEST(deletion, removeOne) {
    HashMap<int> h;

    h.insert(1, 1);

    ASSERT_TRUE(h.contains(1));

    h.erase(1);

    ASSERT_FALSE(h.contains(1));
}

TEST(deletion, exception) {
    HashMap<int> h;

    ASSERT_THROW(h.erase(1), std::logic_error);
}

TEST(deletion, deleteN1) {
    HashMap<int> h;

    for (size_t i = 0; i < N; ++i) {
        h[i] = i;
    }

    for (size_t i = 0; i < N; ++i) {
        ASSERT_TRUE(h.contains(i));

        h.erase(i);

        ASSERT_FALSE(h.contains(i));
    }

    ASSERT_EQ(h.size(), 0);
}

TEST(deletion, deleteN2) {
    HashMap<int> h;

    for (size_t i = 0; i < N; ++i) {
        h[i] = i;

        ASSERT_TRUE(h.contains(i));

        h.erase(i);

        ASSERT_FALSE(h.contains(i));
    }

    ASSERT_EQ(h.size(), 0);
}

TEST(copying, addOneAndCopy) {
    HashMap<int> h1;

    h1.insert(1, 1);

    HashMap<int> h2 = h1;

    ASSERT_EQ(h2[1], 1);
}

TEST(copying, addOneAndMove) {
    HashMap<int> h1;

    h1.insert(1, 1);

    HashMap<int> h2 = h1;

    ASSERT_EQ(h1[1], 1);
    ASSERT_EQ(h1[1], 1);
}
