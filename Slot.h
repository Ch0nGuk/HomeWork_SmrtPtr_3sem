#pragma once

#include <cstddef> 


struct Slot
{
    void* object;
    size_t ref_count;
    size_t next_free; // только когда слот пустой, ссылается на следующий свободный слот
    void (*deleter)(void*); 
};

template <class T>
void delete_object(void* object)
{
    delete static_cast<T*>(object); // приводим объект к указателю на тип T и удаляем
}
