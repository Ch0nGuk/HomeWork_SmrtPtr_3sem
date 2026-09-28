#pragma once

#include "ControlBlock.h"
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
        ShrdPtr() : control_block_(), ptr_() {}
        ShrdPtr(const ShrdPtr& other) noexcept : control_block_(other.control_block_), ptr_(other.ptr_) 
        {
            if (control_block_ != nullptr) control_block_->IncCount();
        }

        template <class U> requires std::derived_from<U, T>
        ShrdPtr(const ShrdPtr<U>& other) noexcept : control_block_(other.control_block_), ptr_(other.ptr_) 
        {
            if (control_block_ != nullptr) control_block_->IncCount();
        }

        ShrdPtr(ShrdPtr&& other) noexcept : control_block_(other.control_block_), ptr_(other.ptr_)
        {
            other.control_block_ = nullptr;
            other.ptr_ = nullptr;
        }

        template <class U> requires std::derived_from<U, T>
        ShrdPtr(ShrdPtr<U>&& other) noexcept : control_block_(other.control_block_), ptr_(other.ptr_)
        {
            other.control_block_ = nullptr;
            other.ptr_ = nullptr;
        }

        ShrdPtr& operator=(const ShrdPtr& other)
        {
            if (this == &other) return *this;

            ShrdPtr tmp_this = *this;
            control_block_ = other.control_block_;
            ptr_ = other.ptr_;
            if (tmp_this.control_block_ != nullptr) tmp_this.control_block_->DecCount(); // уменьшаем счетчик у старого слота
            if (control_block_ != nullptr) control_block_->IncCount(); // увеличиваем счетчик нового слота

            return *this;
        }

        template <class U> requires std::derived_from<U, T>
        ShrdPtr& operator=(const ShrdPtr<U>& other)
        {
            ShrdPtr tmp_this = *this;
            control_block_ = other.control_block_;
            ptr_ = other.ptr_;
            if (tmp_this.control_block_ != nullptr) tmp_this.control_block_->DecCount(); // уменьшаем счетчик у старого слота
            if (control_block_ != nullptr) control_block_->IncCount(); // увеличиваем счетчик нового слота

            return *this;
        }

        ShrdPtr& operator=(ShrdPtr&& other) noexcept
        {
            if (&other == this) return *this;

            ShrdPtr tmp_this = *this;

            control_block_ = other.control_block_;
            ptr_ = other.ptr_;
            other.control_block_ = nullptr;
            if (tmp_this.control_block_ != nullptr) tmp_this.control_block_->DecCount();
            other.ptr_ = nullptr;


            return *this;
        }

        template <class U> requires std::derived_from<U, T>
        ShrdPtr& operator=(ShrdPtr<U>&& other) noexcept
        {
            ShrdPtr tmp_this = *this;

            control_block_ = other.control_block_;
            ptr_ = other.ptr_;
            other.control_block_ = nullptr;
            other.ptr_ = nullptr;
            if (tmp_this.control_block_ != nullptr) tmp_this.control_block_->DecCount();
            
            return *this;
        }

        ~ShrdPtr()
        {
            if (control_block_ != nullptr) control_block_->DecCount();
        }

        T& operator*() const
        {
            if (control_block_ == nullptr) throw std::logic_error("ShrdPtr is empty");
            return *ptr_;
        }

        T* operator->() const
        {
            if (control_block_ == nullptr) throw std::logic_error("ShrdPtr is empty");
            return ptr_;
        }

        explicit operator bool() const noexcept
        {
            return control_block_ != nullptr;
        }

        
    private:
        detail::ControlBlock* control_block_;
        T* ptr_;
        
        ShrdPtr(detail::ControlBlock* new_control_block, T* new_ptr) noexcept : control_block_(new_control_block), ptr_(new_ptr) {}
    };

    template <class T, class... Args>
    ShrdPtr<T> MakeShrdPtr(Args&&... args)
    {
        detail::BlockWithObject<T>* block = new detail::BlockWithObject<T>(std::forward<Args>(args)...);

        return ShrdPtr<T>(block, &(block->object));
    }
} // namespace shrd