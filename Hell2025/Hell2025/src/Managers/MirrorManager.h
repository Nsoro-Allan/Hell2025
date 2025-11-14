#pragma once
#include "HellTypes.h"
#include "Types/Mirror.h"
#include "SlotMap.h"

namespace MirrorManager {
    void AddMirror(uint64_t parentId, uint32_t meshNodeIndex, uint32_t globalMeshIndex);
    void Update();
    void CleanUp();

    Mirror* GetMirrorByObjectId(uint64_t objectId);
    Hell::SlotMap<Mirror>& GetMirrors();
}