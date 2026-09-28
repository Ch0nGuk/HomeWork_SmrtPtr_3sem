#pragma once

#include <cstddef>
#include <utility> // для std::forward


namespace shrd
{
    template <class T> class ShrdPtr;
}

namespace shrd::detail
{

    class ControlBlock
    {
    template <class T> friend class shrd::ShrdPtr;
    public:
        ControlBlock(void (*deleter)(ControlBlock*)) noexcept  ; 
        ~ControlBlock() = default;

        ControlBlock(const ControlBlock&) = delete;
        ControlBlock(ControlBlock&&) = delete;
        ControlBlock& operator=(const ControlBlock&) = delete;
        ControlBlock& operator=(ControlBlock&&) = delete;

    private:
        size_t ref_count_;
        void (*deleter_)(ControlBlock*); 

        void DecCount() noexcept;
        void IncCount() noexcept;
    };

    template <class T> struct BlockWithObject;

    template <class T>
    void delete_block(ControlBlock* control_block)
    {
        delete static_cast<BlockWithObject<T>*>(control_block); // приводим control_block вниз к указателю на блок и удаляем его
    }

    template <class T>
    struct BlockWithObject : ControlBlock
    {
        template <class... Args>
        BlockWithObject(Args&&... args) : ControlBlock(delete_block<T>), object(std::forward<Args>(args)...) {}
        T object;
    };

} // namespace shrd::detil