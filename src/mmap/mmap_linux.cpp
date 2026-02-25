#ifndef _WIN32
#include "mmap.h"

#include <stdexcept>

bool MMap::Trymap(size_t) {
    throw std::runtime_error("MMap::Trymap not implemented on Linux yet");
}

void MMap::Unmap() {}

void MMap::Sync() {}

void MMap::Init_() {
    throw std::runtime_error("MMap::Init_ not implemented on Linux yet");
}
#endif
