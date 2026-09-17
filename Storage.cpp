#include "Storage.h"


namespace smrt::detail
{
    Storage::Storage(void* object, void (*deleter)(void*)) noexcept : object_(object), ref_count_(1), deleter_(deleter) {}
    void Storage::IncCount() noexcept
    {
        ref_count_++;
    }

    void Storage::DecCount() noexcept
    {
        ref_count_--;
        if (ref_count_ == 0)
        {
            deleter_(object_);
            delete this;
        }
    }

} // namespace smrt::detail