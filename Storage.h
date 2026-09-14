#pragma once

#include <cstddef>
#include <limits>
#include <iostream>
#include <cassert>
#include "Slot.h"

template <class T> class SmrtPtr; // нужно для Create
class Storage
{
template <class T> friend class SmrtPtr; // SmrPtr должен иметь доступ к IncCount & DecCount 
public:
    Storage() : slots_(), size_(), capacity_(), free_head_(NO_SLOT) {}

    Storage(const Storage&) = delete; // копирование и перемещение Storage запрещено, потому что
    Storage& operator=(const Storage&) = delete; // 1) это бессмысленно
    Storage(Storage&&) = delete; // 2) при копировании другой объект указывает на тот же массив слотов и может его удалить
    Storage& operator=(Storage&&) = delete;// 3) при перемещении остается висячий указатель в SmrtPtr на хранилище
    
    ~Storage()
    {
        size_t leaks = 0;
        for (size_t index = 0; index < capacity_; index++)
        {
            if (slots_[index].object != nullptr)
            {
                slots_[index].deleter(slots_[index].object);
                leaks++;
            }
        }

        if (leaks) std::cerr << "not all (" << leaks << ") objects were delete\n";
        delete []slots_;
    }
    
    size_t Size() const noexcept
    {
        return size_;
    }

    template <class T, class... Args>
    SmrtPtr<T> Create(Args&&... args);
    //
    
private:
    static const size_t NO_SLOT = std::numeric_limits<size_t>::max(); // статическая константа для обозначения, что free_head_ никуда не указывает
    Slot* slots_;
    size_t size_;
    size_t capacity_;
    size_t free_head_;
    
    void Resize()
    {
        size_t new_capacity = (capacity_ == 0) ? 8 : capacity_ * 2;
        Slot* new_slots = new Slot[new_capacity];
        size_t old_capacity = capacity_;
        capacity_  = new_capacity;
    
        for (size_t index = 0; index < old_capacity; index++) // копируем старые слоты
        {
            new_slots[index] = slots_[index];
        }
        for (size_t index = old_capacity; index < capacity_; index++) // обнуляем новые, связываем их в список пустых.
        {                                                             // последний новый подвязываем на голову пустых, переставляем голву на первый новый 
            new_slots[index].object = nullptr;
            new_slots[index].ref_count = 0;
            if (index != capacity_ - 1) new_slots[index].next_free = index + 1;
            else new_slots[index].next_free = free_head_;
        }
        free_head_ = old_capacity;
    
        delete []slots_;
        slots_ = new_slots;
    }

    size_t ClaimSlot() // функция для занятия слота, выбирает всегда первый свободный. Свободный слот -- предусловие
    {
        assert(free_head_ != NO_SLOT && "No free slots");
        size_t first_free = free_head_;
        free_head_ = slots_[free_head_].next_free;
        size_++;
        
        return first_free;
    }

    void FreeSlot(size_t id) // функция для освобождения слота
    {
        assert(id < capacity_ && "ID out of range");
        assert(slots_[id].object != nullptr && "Slot is already free");

        slots_[id].object = nullptr;
        slots_[id].ref_count = 0;
        slots_[id].next_free = free_head_;
        free_head_ = id;

        size_--;
    }

    void IncCount(size_t id) // increment count -- функция-увеличитель счетчика
    {
        assert(id < capacity_ && "ID out of range");
        assert(slots_[id].object != nullptr && "Slots is free");

        slots_[id].ref_count++;
    }

    void DecCount(size_t id) // decrement count -- функция -уменьшитель счетчика и удалитель в случае последнего владельца
    {
        assert(id < capacity_ && "ID out of range");
        assert(slots_[id].object != nullptr && "Slots is free");
        assert(slots_[id].ref_count > 0 && "Slot's ref_count is zero");

        slots_[id].ref_count--;
        if (slots_[id].ref_count == 0)
        {
            slots_[id].deleter(slots_[id].object);
            FreeSlot(id);
        }
    }
};