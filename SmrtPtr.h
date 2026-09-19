#pragma once

#include "Storage.h"
#include <utility> // для std::forward
#include <stdexcept>
#include <concepts> // для std::derived_from

namespace smrt
{
    template <class T, class... Args> SmrtPtr<T> MakeSmrtPtr(Args&&...);


    template <class T>
    class SmrtPtr
    {
    template <class U, class... Args>   friend SmrtPtr<U> MakeSmrtPtr(Args&&...);
    template <class U> friend class SmrtPtr;
    public:
        SmrtPtr() : store_(), ptr_() {}
        SmrtPtr(const SmrtPtr& other) noexcept : store_(other.store_), ptr_(other.ptr_) 
        {
            if (store_ != nullptr) store_->IncCount();
        }

        template <class U> requires std::derived_from<U, T>
        SmrtPtr(const SmrtPtr<U>& other) noexcept : store_(other.store_), ptr_(other.ptr_) 
        {
            if (store_ != nullptr) store_->IncCount();
        }

        SmrtPtr(SmrtPtr&& other) noexcept : store_(other.store_), ptr_(other.ptr_)
        {
            other.store_ = nullptr;
            other.ptr_ = nullptr;
        }

        template <class U> requires std::derived_from<U, T>
        SmrtPtr(SmrtPtr<U>&& other) noexcept : store_(other.store_), ptr_(other.ptr_)
        {
            other.store_ = nullptr;
            other.ptr_ = nullptr;
        }

        SmrtPtr& operator=(const SmrtPtr& other)
        {
            if (this == &other) return *this;

            SmrtPtr tmp_this = *this;
            store_ = other.store_;
            ptr_ = other.ptr_;
            if (tmp_this.store_ != nullptr) tmp_this.store_->DecCount(); // уменьшаем счетчик у старого слота
            if (store_ != nullptr) store_->IncCount(); // увеличиваем счетчик нового слота

            return *this;
        }

        template <class U> requires std::derived_from<U, T>
        SmrtPtr& operator=(const SmrtPtr<U>& other)
        {
            SmrtPtr tmp_this = *this;
            store_ = other.store_;
            ptr_ = other.ptr_;
            if (tmp_this.store_ != nullptr) tmp_this.store_->DecCount(); // уменьшаем счетчик у старого слота
            if (store_ != nullptr) store_->IncCount(); // увеличиваем счетчик нового слота

            return *this;
        }

        SmrtPtr& operator=(SmrtPtr&& other) noexcept
        {
            if (&other == this) return *this;

            SmrtPtr tmp_this = *this;

            store_ = other.store_;
            ptr_ = other.ptr_;
            other.store_ = nullptr;
            if (tmp_this.store_ != nullptr) tmp_this.store_->DecCount();
            other.ptr_ = nullptr;


            return *this;
        }

        template <class U> requires std::derived_from<U, T>
        SmrtPtr& operator=(SmrtPtr<U>&& other) noexcept
        {
            SmrtPtr tmp_this = *this;

            store_ = other.store_;
            ptr_ = other.ptr_;
            other.store_ = nullptr;
            other.ptr_ = nullptr;
            if (tmp_this.store_ != nullptr) tmp_this.store_->DecCount();
            
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
        
        SmrtPtr(detail::Storage* new_store, T* new_ptr) noexcept : store_(new_store), ptr_(new_ptr) {}
    };

    template <class T, class... Args>
    SmrtPtr<T> MakeSmrtPtr(Args&&... args)
    {
        T* ptr = new T(std::forward<Args>(args)...);
        detail::Storage* store = new detail::Storage(ptr, detail::delete_object<T>);

        return SmrtPtr<T>(store, ptr);
    }
} // namespace smrt