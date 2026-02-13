#include <iostream>
#include <filesystem>
#include <cstring>
#include <stdexcept>

#include "mmap.h"
MMap::MMap(const std::filesystem::path& filePath) : filePath_(filePath), handle_(nullptr), capacity_(0) {
    Init_();
}
void MMap::resize(size_t new_size)
{
    Reserve(new_size);
    Header()->size = static_cast<uint32_t>(new_size);
    Sync();
}
size_t MMap::size() const{
    return Header()->size;
}
uint8_t* MMap::data(){
    return static_cast<uint8_t*>(handle_) + sizeof(MMapHeader);
}
void MMap::clear(){
    resize(0);
}
void MMap::push(const uint8_t* input, size_t length){
    size_t current_size = size();
    resize(current_size + length);
    std::memcpy(data() + current_size, input, length);
}
double MMap::get_ratio() const{
    if(Capacity() == 0) {
        return 0.0;
    }
    return static_cast<double>(size()) / static_cast<double>(Capacity());
}
bool MMap::empty() const{
    return size() == 0;
}
void MMap::Reserve(size_t new_size){
    if(new_size > capacity_){
        size_t new_capacity = capacity_ == 0 ? 4096 : capacity_;
        while(new_capacity < new_size){
            new_capacity *= 2;
        }
        EnsureCapacity(new_capacity);
    }
}
void MMap::EnsureCapacity(size_t new_size){
    if(handle_){
        Sync();
        Unmap();
    }
    if(!Trymap(new_size)){
        throw std::runtime_error("Failed to remap file");
    }
    capacity_ = new_size;
}
size_t MMap::Capacity() const{
    return capacity_ - sizeof(MMapHeader);
}
bool MMap::Isvalid() const{
    return Header() && Header()->magic == MMapHeader::MAGIC;
}
MMap::MMapHeader* MMap::Header() const
{
    return static_cast<MMapHeader*>(handle_);
}
