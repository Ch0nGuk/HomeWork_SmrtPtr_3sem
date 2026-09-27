#pragma once

#include "Storage.h"
#include <utility> // для std::forward
#include <stdexcept>
#include <concepts> // для std::derived_from

namespace shrd
{
    template <class T, class... Args> ShrdPtr<T> MakeShrdPtr(Args&&...);


    template <class T>
    class ShrdPtr
    {
    template <class U, class... Args>   friend ShrdPtr<U> MakeShrdPtr(Args&&...);
    template <class U> friend class ShrdPtr;
    public:
        ShrdPtr() : store_(), ptr_() {}
        ShrdPtr(const ShrdPtr& other) noexcept : store_(other.store_), ptr_(other.ptr_) 
        {
            if (store_ != nullptr) store_->IncCount();
        }

        template <class U> requires std::derived_from<U, T>
        ShrdPtr(const ShrdPtr<U>& other) noexcept : store_(other.store_), ptr_(other.ptr_) 
        {
            if (store_ != nullptr) store_->IncCount();
        }

        ShrdPtr(ShrdPtr&& other) noexcept : store_(other.store_), ptr_(other.ptr_)
        {
            other.store_ = nullptr;
            other.ptr_ = nullptr;
        }

        template <class U> requires std::derived_from<U, T>
        ShrdPtr(ShrdPtr<U>&& other) noexcept : store_(other.store_), ptr_(other.ptr_)
        {
            other.store_ = nullptr;
            other.ptr_ = nullptr;
        }

        ShrdPtr& operator=(const ShrdPtr& other)
        {
            if (this == &other) return *this;

            ShrdPtr tmp_this = *this;
            store_ = other.store_;
            ptr_ = other.ptr_;
            if (tmp_this.store_ != nullptr) tmp_this.store_->DecCount(); // уменьшаем счетчик у старого слота
            if (store_ != nullptr) store_->IncCount(); // увеличиваем счетчик нового слота

            return *this;
        }

        template <class U> requires std::derived_from<U, T>
        ShrdPtr& operator=(const ShrdPtr<U>& other)
        {
            ShrdPtr tmp_this = *this;
            store_ = other.store_;
            ptr_ = other.ptr_;
            if (tmp_this.store_ != nullptr) tmp_this.store_->DecCount(); // уменьшаем счетчик у старого слота
            if (store_ != nullptr) store_->IncCount(); // увеличиваем счетчик нового слота

            return *this;
        }

        ShrdPtr& operator=(ShrdPtr&& other) noexcept
        {
            if (&other == this) return *this;

            ShrdPtr tmp_this = *this;

            store_ = other.store_;
            ptr_ = other.ptr_;
            other.store_ = nullptr;
            if (tmp_this.store_ != nullptr) tmp_this.store_->DecCount();
            other.ptr_ = nullptr;


            return *this;
        }

        template <class U> requires std::derived_from<U, T>
        ShrdPtr& operator=(ShrdPtr<U>&& other) noexcept
        {
            ShrdPtr tmp_this = *this;

            store_ = other.store_;
            ptr_ = other.ptr_;
            other.store_ = nullptr;
            other.ptr_ = nullptr;
            if (tmp_this.store_ != nullptr) tmp_this.store_->DecCount();
            
            return *this;
        }

        ~ShrdPtr()
        {
            if (store_ != nullptr) store_->DecCount();
        }

        T& operator*() const
        {
            if (store_ == nullptr) throw std::logic_error("ShrdPtr is empty");
            return *ptr_;
        }

        T* operator->() const
        {
            if (store_ == nullptr) throw std::logic_error("ShrdPtr is empty");
            return ptr_;
        }

        explicit operator bool() const noexcept
        {
            return store_ != nullptr;
        }

        
    private:
        detail::Storage* store_;
        T* ptr_;
        
        ShrdPtr(detail::Storage* new_store, T* new_ptr) noexcept : store_(new_store), ptr_(new_ptr) {}
    };

    template <class T, class... Args>
    ShrdPtr<T> MakeShrdPtr(Args&&... args)
    {
        detail::Block<T>* block = new detail::Block<T>(std::forward<Args>(args)...);

        return ShrdPtr<T>(block, &(block->object));
    }
} // namespace shrd