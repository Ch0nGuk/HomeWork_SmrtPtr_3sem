#include "Storage.h"


namespace shrd::detail
{
    Storage::Storage(void (*deleter)(Storage*)) noexcept : ref_count_(1), deleter_(deleter) {}
    void Storage::IncCount() noexcept
    {
        ref_count_++;
    }

    void Storage::DecCount() noexcept
    {
        ref_count_--;
        if (ref_count_ == 0)
        {
            deleter_(this);
        }
    }

} // namespace shrd::detail