#pragma once
#include "HellDefines.h"
#include "HellEnums.h"
#include "HellTypes.h"
#include "Handlers/Openable.h"

namespace OpenableManager {
    uint64_t CreateOpenable();
    void Update(float deltaTime);
    void TriggerInteract(uint64_t handlerId);

    bool OpenableExists(uint64_t handlerId);
    Openable* GetOpeneableByOpenableId(uint64_t handlerId);
    //Transform GetOpenStateHandleTransformById(uint64_t hanlderId);
}