#ifndef _WIN32
#include "mmap.h"

#include <algorithm>
#include <cerrno>
#include <cstring>
#include <stdexcept>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

namespace {
size_t page_size() {
    long ps = ::sysconf(_SC_PAGESIZE);
    return ps > 0 ? static_cast<size_t>(ps) : 4096;
}
size_t align_up(size_t n, size_t align) {
    return (n + align - 1) / align * align;
}
}

bool MMap::Trymap(size_t) {
    throw std::runtime_error("MMap::Trymap not implemented on Linux yet");
}

void MMap::Unmap() {}

void MMap::Sync() {}

void MMap::Init_() {
    throw std::runtime_error("MMap::Init_ not implemented on Linux yet");
}
#endif
