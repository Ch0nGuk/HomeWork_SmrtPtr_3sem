#pragma once

#include <cstddef>
#include <utility> // для std::forward


namespace shrd
{
    template <class T> class ShrdPtr;
}

namespace shrd::detail
{

    class Storage
    {
    template <class T> friend class shrd::ShrdPtr;
    public:
        Storage(void (*deleter)(Storage*)) noexcept  ; 
        ~Storage() = default;

        Storage(const Storage&) = delete;
        Storage(Storage&&) = delete;
        Storage& operator=(const Storage&) = delete;
        Storage& operator=(Storage&&) = delete;

    private:
        size_t ref_count_;
        void (*deleter_)(Storage*); 

        void DecCount() noexcept;
        void IncCount() noexcept;
    };

    template <class T> struct Block;

    template <class T>
    void delete_block(Storage* store)
    {
        delete static_cast<Block<T>*>(store); // приводим store вниз к указателю на блок и удаляем его
    }

    template <class T>
    struct Block : Storage
    {
        template <class... Args>
        Block(Args&&... args) : Storage(delete_block<T>), object(std::forward<Args>(args)...) {}
        T object;
    };

} // namespace shrd::detil