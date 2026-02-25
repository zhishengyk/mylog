#include "mmap/mmap.h"

#include <gtest/gtest.h>

#include <cstring>
#include <filesystem>
#include <vector>

namespace {
std::filesystem::path temp_path() {
    return std::filesystem::path("tests") / "tmp_mmap.bin";
}
}  // namespace

TEST(MMapTest, WriteRead) {
    auto p = temp_path();
    std::filesystem::create_directories(p.parent_path());
    if (std::filesystem::exists(p)) {
        std::filesystem::remove(p);
    }

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

    std::filesystem::remove(p);
}

TEST(MMapTest, Expand) {
    auto p = temp_path();
    std::filesystem::create_directories(p.parent_path());
    if (std::filesystem::exists(p)) {
        std::filesystem::remove(p);
    }

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

    std::filesystem::remove(p);
}
