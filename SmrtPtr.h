#pragma once

#include "Storage.h"
#include <cstddef>
#include <utility> 
#include <stdexcept>

namespace smrt
{
    template <class T>
    class SmrtPtr
    {
    public:
        SmrtPtr() : store_(), ptr_() {}
        SmrtPtr(const SmrtPtr& other) noexcept : store_(other.store_), ptr_(other.ptr_) 
        {
            if (store_ != nullptr) store_->IncCount();
        }
        SmrtPtr(SmrtPtr&& other) noexcept : store_(other.store_), ptr_(other.ptr_)
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
            if (store_ != nullptr) store_->DecCount();
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
        detail::Storage* store_;
        T* ptr_;

        SmrtPtr(Storage* new_store, size_t new_id, T* new_ptr) noexcept : store_(new_store), id_(new_id), ptr_(new_ptr) {}
    };
} // namespace smrt