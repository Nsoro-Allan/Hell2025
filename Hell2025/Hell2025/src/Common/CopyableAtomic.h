#include <atomic>

template <class T>
struct CopyableAtomic {
    std::atomic<T> v;

    CopyableAtomic() noexcept = default;
    constexpr CopyableAtomic(T init) noexcept : v(init) {}

    // Copy/move just transfer the current value
    CopyableAtomic(const CopyableAtomic& rhs) noexcept
        : v(rhs.v.load(std::memory_order_relaxed)) {
    }

    CopyableAtomic& operator=(const CopyableAtomic& rhs) noexcept {
        v.store(rhs.v.load(std::memory_order_relaxed), std::memory_order_relaxed);
        return *this;
    }

    CopyableAtomic(CopyableAtomic&& rhs) noexcept
        : v(rhs.v.load(std::memory_order_relaxed)) {
    }

    CopyableAtomic& operator=(CopyableAtomic&& rhs) noexcept {
        v.store(rhs.v.load(std::memory_order_relaxed), std::memory_order_relaxed);
        return *this;
    }

    void store(T x, std::memory_order o = std::memory_order_seq_cst) noexcept { 
        v.store(x, o); 
    }
    
    T load(std::memory_order o = std::memory_order_seq_cst) const noexcept { 
        return v.load(o); 
    }
};