#pragma once
#include <cstdint>

enum class ObjectType : uint16_t;

namespace UniqueID {

    inline constexpr uint32_t kTypeBits = 16;
    inline constexpr uint64_t kTypeShift = 64 - kTypeBits;
    inline constexpr uint64_t kTypeMask = ((1ull << kTypeBits) - 1) << kTypeShift;
    inline constexpr uint64_t kLocalMask = ~kTypeMask;

    inline uint64_t MakeId(ObjectType type, uint64_t local) {
        return (uint64_t(static_cast<uint16_t>(type)) << kTypeShift) | (local & kLocalMask);
    }
    
    inline ObjectType GetType(uint64_t id) {
        return static_cast<ObjectType>((id >> kTypeShift) & ((1ull << kTypeBits) - 1));
    }

    inline uint64_t GetLocal(uint64_t id) {
        return id & kLocalMask;
    }

    uint64_t GetNextGlobal();
    uint64_t GetNext(ObjectType type);

}
