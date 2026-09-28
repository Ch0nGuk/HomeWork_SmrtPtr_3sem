#include "ControlBlock.h"


namespace shrd::detail
{
    ControlBlock::ControlBlock(void (*deleter)(ControlBlock*)) noexcept : ref_count_(1), deleter_(deleter) {}
    void ControlBlock::IncCount() noexcept
    {
        ref_count_++;
    }

    void ControlBlock::DecCount() noexcept
    {
        ref_count_--;
        if (ref_count_ == 0)
        {
            deleter_(this);
        }
    }

} // namespace shrd::detail