#include "UniqueID.h"
#include <atomic>
#include <vector>

namespace {
    std::atomic<uint64_t> g_global{ 1 };

    struct PerType {
        std::vector<std::atomic<uint64_t>> ctr;
        PerType() : ctr(size_t(1ull << UniqueID::kTypeBits)) {
            for (auto& a : ctr) a.store(1, std::memory_order_relaxed);
        }
    };
    PerType& Table() { static PerType t; return t; }
}

namespace UniqueID {
    uint64_t GetNextGlobal() {
        return g_global.fetch_add(1, std::memory_order_relaxed);
    }

    uint64_t GetNext(ObjectType type) {
        const auto idx = size_t(static_cast<uint16_t>(type));
        const uint64_t local = Table().ctr[idx].fetch_add(1, std::memory_order_relaxed);
        return MakeId(type, local);
    }
}
