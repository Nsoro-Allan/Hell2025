#include "MirrorManager.h"
#include "UniqueID.h"
#include "World/World.h"

namespace MirrorManager {
    Hell::SlotMap<Mirror> g_mirrors;

    void AddMirror(uint64_t parentId, uint32_t meshNodeIndex, uint32_t globalMeshIndex) {
        const uint64_t id = UniqueID::GetNextObjectId(ObjectType::MIRROR);
        g_mirrors.emplace_with_id(id, id, parentId, meshNodeIndex, globalMeshIndex);
    }

    void Update() {
        for (Mirror& mirror : g_mirrors) {
            if (GenericObject* genericObject = World::GetGenericObjectById(mirror.GetParentId())) {
                const MeshNodes& meshNodes = genericObject->GetMeshNodes();
                const glm::mat4& worldMatrix = meshNodes.GetWorldModelMatrix(mirror.GetMeshNodeIndex());
                mirror.Update(worldMatrix);
            }
        }
    }

    void CleanUp() {
        g_mirrors.clear();
    }

    Mirror* GetMirrorByObjectId(uint64_t objectId) {
        return g_mirrors.get(objectId);
    }

    Hell::SlotMap<Mirror>& GetMirrors() {
        return g_mirrors;
    }
}