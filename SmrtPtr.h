#pragma once

#include "Storage.h"
#include <cstddef>
#include <utility> 
#include <stdexcept>


template <class T>
class SmrtPtr
{
friend class Storage; // чтобы вызывать приватный конструктор SmrtPtr в Create у Storage
template <class U> friend class SmrtPtr; // для апкаста 
public:
    SmrtPtr() : store_(), id_(), ptr_() {}
    SmrtPtr(const SmrtPtr& other) noexcept : store_(other.store_), id_(other.id_), ptr_(other.ptr_) 
    {
        if (store_ != nullptr) store_->IncCount(id_);
    }
    SmrtPtr(SmrtPtr&& other) noexcept : store_(other.store_), id_(other.id_), ptr_(other.ptr_)
    {
        other.store_ = nullptr;
        other.ptr_ = nullptr;
    }
    SmrtPtr& operator=(const SmrtPtr& other)
    {
        if (this == &other) return *this;

        if (store_ != nullptr) store_->DecCount(id_); // уменьшаем счетчик у старого слота
        store_ = other.store_;
        id_ = other.id_;
        ptr_ = other.ptr_;
        if (store_ != nullptr) store_->IncCount(id_); // увеличиваем счетчик нового слота

        return *this;
    }
    SmrtPtr& operator=(SmrtPtr&& other) noexcept
    {
        if (&other == this) return *this;

        if (this->store_ != nullptr) store_->DecCount(id_);
        store_ = other.store_;
        id_ = other.id_;
        ptr_ = other.ptr_;
        other.store_ = nullptr;
        other.ptr_ = nullptr;

        return *this;
    }
    ~SmrtPtr()
    {
        if (store_ != nullptr) store_->DecCount(id_);
    }

    T& operator*() const
    {
        if (store_ == nullptr) throw std::logic_error("SmrtPtr is empty");
        return *ptr_;
    }

    T* operator->() const
    {
        if (store_ == nullptr) throw std::logic_error("SmrtPtr is empty");
        return ptr_;
    }

    explicit operator bool() const noexcept
    {
        return store_ != nullptr;
    }

private:
    Storage* store_;
    size_t id_;
    T* ptr_;

    SmrtPtr(Storage* new_store, size_t new_id, T* new_ptr) noexcept : store_(new_store), id_(new_id), ptr_(new_ptr) {}
};

template <class T, class... Args>
SmrtPtr<T> Storage::Create(Args&& ...args)
{
    if (free_head_ == NO_SLOT) Resize();

    T* tmp = new T(std::forward<Args>(args)...);
    size_t first_free = ClaimSlot();
    slots_[first_free].object = tmp;
    slots_[first_free].ref_count = 1;
    slots_[first_free].deleter = delete_object<T>;

    return SmrtPtr<T>(this, first_free, tmp); // this -- адрес хранилища (это ведь его метол)
}