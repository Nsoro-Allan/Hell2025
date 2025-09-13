#pragma once
#include "HellDefines.h"
#include "HellEnums.h"
#include "HellTypes.h"
#include "Handlers/OpenHandler.h"

namespace OpenStateHandlerManager {
    uint64_t CreateOpenStateHandler();
    void Update(float deltaTime);
    void TriggerInteract(uint64_t handlerId);

    bool OpenStateHandlerExists(uint64_t handlerId);
    OpenStateHandler* GetOpenStateHandlerByHandlerId(uint64_t handlerId);
    Transform GetOpenStateHandleTransformById(uint64_t hanlderId);

}