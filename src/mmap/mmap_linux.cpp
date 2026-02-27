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

bool MMap::Trymap(size_t capacity) {
    size_t need = std::max(capacity, sizeof(MMapHeader));
    size_t map_size = align_up(need, page_size());
    if (handle_) {
        ::msync(handle_, capacity_, MS_SYNC);
        ::munmap(handle_, capacity_);
        handle_ = nullptr;
        capacity_ = 0;
    }

    if(::ftruncate(fd_, static_cast<off_t>(map_size)) != 0) {
       return false;
    }

    void* p = ::mmap(nullptr, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
    if (p == MAP_FAILED) {
       return false;
    }
    handle_ = p;
    capacity_ = map_size;
    return true;
}

void MMap::Unmap() {
    if(handle_){
        ::msync(handle_, capacity_, MS_SYNC);
        ::munmap(handle_, capacity_);
        handle_ = nullptr;
        capacity_ = 0;
    }
}

void MMap::Sync() {
    if(handle_){
        ::msync(handle_, capacity_, MS_SYNC);
    }
}

void MMap::Init_() {
    fd_ = ::open(filePath_.c_str(), O_RDWR | O_CREAT, 0644);
    if (fd_ < 0) {
        throw std::runtime_error("Failed to open file: " + filePath_.string() + ", error: " + std::strerror(errno));
    }
    struct stat st{};
    if (::fstat(fd_, &st) != 0) {
        ::close(fd_);
        fd_ = -1;
        throw std::runtime_error("Failed to stat file: " + filePath_.string() + ", error: " + std::strerror(errno));
    }
    size_t file_size = static_cast<size_t>(st.st_size);
    size_t need = std::max(file_size, sizeof(MMapHeader));
    size_t map_size = align_up(need, page_size());
    if (!Trymap(map_size)) {
        ::close(fd_);
        fd_ = -1;
        throw std::runtime_error("Failed to map file: " + filePath_.string());
    }

    if(!Isvalid()) {
        Header()->magic = MMapHeader::MAGIC;
        Header()->size = 0;
        Sync();
    }
}
#endif
