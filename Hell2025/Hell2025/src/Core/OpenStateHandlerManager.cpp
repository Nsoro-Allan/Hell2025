#include "OpenStateHandlerManager.h"
#include "UniqueID.h"

namespace OpenStateHandlerManager {

    std::unordered_map<uint64_t, OpenStateHandler> g_openStateHandlers;

    uint64_t CreateOpenStateHandler() {
        uint64_t handlerId = UniqueID::GetNext();
        OpenStateHandler& openStateHandler = g_openStateHandlers[handlerId];

        return handlerId;
    }

    void Update(float deltaTime) {
        for (auto& pair : g_openStateHandlers) {
            uint64_t handlerId = pair.first;
            OpenStateHandler& openStateHandler = pair.second;

            openStateHandler.Update(deltaTime);
        }
    }

    void TriggerInteract(uint64_t handlerId) {
        if (!OpenStateHandlerExists(handlerId)) return;

        OpenStateHandler* openStateHandler = GetOpenStateHandlerByHandlerId(handlerId);
        return openStateHandler->Interact();
    }

    bool OpenStateHandlerExists(uint64_t handlerId) {
        return (g_openStateHandlers.find(handlerId) != g_openStateHandlers.end());
    }

    OpenStateHandler* GetOpenStateHandlerByHandlerId(uint64_t handlerId) {
        if (OpenStateHandlerExists(handlerId)) {
            return &g_openStateHandlers[handlerId];
        }
        else {
            return nullptr;
        }
    }

    Transform GetOpenStateHandleTransformById(uint64_t handlerId) {
        if (!OpenStateHandlerExists(handlerId)) return Transform();

        OpenStateHandler* openStateHandler = GetOpenStateHandlerByHandlerId(handlerId);
        return openStateHandler->m_transform;
    }
}