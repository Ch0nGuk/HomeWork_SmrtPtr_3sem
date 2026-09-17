#pragma once

#include <cstddef>


namespace smrt
{
    template <class T> class SmrtPtr;
}

namespace smrt::detail
{
    class Storage
    {
    template <class T> friend class smrt::SmrtPtr;
    public:
        Storage(void* object, void (*deleter)(void*)) noexcept  ; 
        ~Storage() = default;

        Storage(const Storage&) = delete;
        Storage(Storage&&) = delete;
        Storage& operator=(const Storage&) = delete;
        Storage& operator=(Storage&&) = delete;

    private:
        void* object_;
        size_t ref_count_;
        void (*deleter_)(void*); 

        void DecCount() noexcept;
        void IncCount() noexcept;
    };

    template <class T>
    void delete_object(void* object)
    {
        delete static_cast<T*>(object); // приводим объект к указателю на тип T и удаляем
    }
} // namespace smrt::detil