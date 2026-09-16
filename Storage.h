#pragma once

#include <cstddef>

namespace smrt::detail
{

class Storage
{
template <class T> class SmrtPtr friend;    
public:
    Storage(const Storage&) = delete;
    Storage(Storage&&) = delete;
    Storage& operator=(const Storage&) = delete;
    Storage& operator=(Storage&&) = delete;
private:
    void* object_;
    size_t ref_count_;
    void (*deleter)(void*); 
    Storage(void* object, void (*deleter)(void*)); 

    void DecCount();
    void IncCount();
};

template <class T>
void delete_object(void* object)
{
    delete static_cast<T*>(object); // приводим объект к указателю на тип T и удаляем
}


} 