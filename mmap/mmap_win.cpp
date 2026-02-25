#ifdef _WIN32
#include "mmap.h"

#include <stdexcept>
namespace {
    size_t page_size(){
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        return static_cast<size_t>(si.dwPageSize);
    }
    size_t align_up(size_t n, size_t align) {
        return (n + align - 1) / align * align;
    }
}

void MMap::Init_() {
    hFile_ = nullptr;
    hMap_ = nullptr;
    handle_ = nullptr;
    capacity_ = 0;

    hFile_ = CreateFileW(filePath_.wstring().c_str(), GENERIC_READ | GENERIC_WRITE,
    FILE_SHARE_READ | FILE_SHARE_WRITE,
    nullptr,
    OPEN_ALWAYS,
    FILE_ATTRIBUTE_NORMAL,
    nullptr);

    if (hFile_ == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("Failed to create file handle");
    }
    
    LARGE_INTEGER li{};
    if (!GetFileSizeEx(hFile_, &li)) {
        CloseHandle(hFile_);
        throw std::runtime_error("Failed to get file size");
    }

    size_t file_size = static_cast<size_t>(li.QuadPart);
    size_t need = std::max(file_size, sizeof(MMapHeader));
    size_t map_size = align_up(need, page_size());

    LARGE_INTEGER end{};
    end.QuadPart = static_cast<LONGLONG>(map_size);
    if (!SetFilePointerEx(hFile_, end, nullptr, FILE_BEGIN) || !
        SetEndOfFile(hFile_)) {
        CloseHandle(hFile_);
        throw std::runtime_error("Failed to set file size");
    }
     hMap_ = CreateFileMappingW(
        hFile_, NULL, PAGE_READWRITE,
        static_cast<DWORD>(map_size >> 32),
        static_cast<DWORD>(map_size & 0xFFFFFFFF),
        NULL
    );
    if (!hMap_) {
        CloseHandle(hFile_);
        hFile_ = NULL;
        throw std::runtime_error("CreateFileMappingW failed");
    }

    handle_ = MapViewOfFile(hMap_, FILE_MAP_ALL_ACCESS, 0, 0, map_size);
    if (!handle_) {
        CloseHandle(hMap_);
        CloseHandle(hFile_);
        hMap_ = NULL;
        hFile_ = NULL;
        throw std::runtime_error("MapViewOfFile failed");
    }

    capacity_ = map_size;

    if (!Isvalid()) {
        Header()->magic = MMapHeader::MAGIC;
        Header()->size = 0;
        Sync();
    }



}
bool MMap::Trymap(size_t) {
    throw std::runtime_error("MMap::Trymap not implemented on Windows yet");
}

void MMap::Unmap() {}

void MMap::Sync() {}


#endif
