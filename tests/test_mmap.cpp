#include "mmap/mmap.h"

#include <gtest/gtest.h>

#include <cstring>
#include <filesystem>
#include <vector>
#include <system_error>

namespace {
std::filesystem::path temp_path() {
    return std::filesystem::path("tests") / "tmp_mmap.bin";
}

void remove_file(const std::filesystem::path& p) {
    std::error_code ec;
    std::filesystem::remove(p, ec);
}

void ensure_clean(const std::filesystem::path& p) {
    std::filesystem::create_directories(p.parent_path());
    remove_file(p);
}
}  // namespace

TEST(MMapTest, WriteRead) {
    auto p = temp_path();
    ensure_clean(p);

    {
        MMap m(p);
        const char* msg = "hello mmap";
        m.push(reinterpret_cast<const uint8_t*>(msg), std::strlen(msg));
        ASSERT_EQ(m.size(), std::strlen(msg));
    }

    {
        MMap m(p);
        ASSERT_EQ(m.size(), std::strlen("hello mmap"));
        std::vector<uint8_t> buf(m.size());
        std::memcpy(buf.data(), m.data(), m.size());
        ASSERT_EQ(std::memcmp(buf.data(), "hello mmap", m.size()), 0);
    }

    remove_file(p);
}

TEST(MMapTest, Expand) {
    auto p = temp_path();
    ensure_clean(p);

    std::vector<uint8_t> data(10000, 0x5A);
    {
        MMap m(p);
        m.push(data.data(), data.size());
        ASSERT_EQ(m.size(), data.size());
    }

    {
        MMap m(p);
        ASSERT_EQ(m.size(), data.size());
        ASSERT_EQ(std::memcmp(m.data(), data.data(), data.size()), 0);
    }

    remove_file(p);
}

TEST(MMapTest, ClearAndEmpty) {
    auto p = temp_path();
    ensure_clean(p);

    {
        MMap m(p);
        const char* msg = "data";
        m.push(reinterpret_cast<const uint8_t*>(msg), std::strlen(msg));
        ASSERT_FALSE(m.empty());
        m.clear();
        ASSERT_TRUE(m.empty());
        ASSERT_EQ(m.size(), 0u);
        ASSERT_EQ(m.get_ratio(), 0.0);
    }

    remove_file(p);
}

TEST(MMapTest, ResizeAndRatio) {
    auto p = temp_path();
    ensure_clean(p);

    {
        MMap m(p);
        m.resize(128);
        ASSERT_EQ(m.size(), 128u);
        EXPECT_GT(m.get_ratio(), 0.0);
        EXPECT_LE(m.get_ratio(), 1.0);
        m.resize(0);
        ASSERT_EQ(m.size(), 0u);
        ASSERT_TRUE(m.empty());
    }

    remove_file(p);
}

TEST(MMapTest, MultiplePush) {
    auto p = temp_path();
    ensure_clean(p);

    {
        MMap m(p);
        const char* a = "hello";
        const char* b = "world";
        m.push(reinterpret_cast<const uint8_t*>(a), std::strlen(a));
        m.push(reinterpret_cast<const uint8_t*>(b), std::strlen(b));
        ASSERT_EQ(m.size(), std::strlen(a) + std::strlen(b));

        std::vector<uint8_t> buf(m.size());
        std::memcpy(buf.data(), m.data(), m.size());
        ASSERT_EQ(std::memcmp(buf.data(), "helloworld", m.size()), 0);
    }

    remove_file(p);
}

TEST(MMapTest, ZeroLengthPush) {
    auto p = temp_path();
    ensure_clean(p);

    {
        MMap m(p);
        const char* msg = "abc";
        m.push(reinterpret_cast<const uint8_t*>(msg), std::strlen(msg));
        size_t before = m.size();
        m.push(reinterpret_cast<const uint8_t*>(msg), 0);
        ASSERT_EQ(m.size(), before);
    }

    remove_file(p);
}
