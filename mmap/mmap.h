#include <iostream>
#include <filesystem>
class MMap {
public:
    MMap(const std::filesystem::path& filePath) : filePath_(filePath) {
        // Constructor implementation
    }
    ~MMap() = default; 

    MMap(const MMap&) = delete;
    
    MMap& operator=(const MMap&) = delete;
    
    void resize(size_t new_size);
    
    size_t size() const;

    uint8_t* data();

    void clear();

    void push(const uint8_t* data, size_t length);

    double get_ratio() const;

    bool empty() const;
private:
    struct MMapHeader { 
        static constexpr uint32_t MAGIC = 0x4D4D4150; // 'MMAP'
        uint32_t magic = MAGIC;
        uint32_t size = 0;
    }
    void Reserve(size_t new_size);

    void EnsureCapacity(size_t new_size);
    
    size_t Capacity() const;

    bool Trymap(size_t capacity);

    void Unmap();

    void Sync();

    bool Isvalid() const;
    
    MMap* Header() const;

    void Init_();
    std::filesystem::path filePath_;
    void* handle_;
    size_t capacity_;
}