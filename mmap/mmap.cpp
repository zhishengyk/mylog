#include <iostream>
#include <filesystem>
#include "mmap.h"
MMap::resize(size_t new_size)
{
    Reserve(new_size);
    Header()->size = static_cast<uint32_t>(new_size);
    Sync();
}
MMap::size() const{
    return Header()->size;
}
MMap::data(){
    return static_cast<uint8_t*>(handle_) + sizeof(MMapHeader);
}
MMap::clear(){
    resize(0);
}
MMap::push(const uint8_t* data, size_t length){
    size_t current_size = size();
    resize(current_size + length);
    std::memcpy(data() + current_size, data, length);
}
MMap::Reserve(size_t new_size)
{
    if(new_size > capacity_){
        size_t new_capacity = capacity_ == 0 ? 4096 : capacity_;
        while(new_capacity < new_size){
            new_capacity *= 2;
        }
        EnsureCapacity(new_capacity);
    }
}
MMap::EnsureCapacity(size_t new_size)
{
    if(handle_){
        Sync();
        Unmap();
    }
    if(!Trymap(new_size)){
        throw std::runtime_error("Failed to remap file");
    }
    capacity_ = new_size;
}
MMap::Trymap(size_t capacity)
{
    // Platform-specific mmap implementation goes here
    return true; // Placeholder
}
MMap::Header() const
{
    return static_cast<MMapHeader*>(handle_);
}
MMap::Unmap(){

}
MMap::Sync(){

}
