#include "DeadLock.h"
#include "World/World.h"

void DeadLock::Init(uint64_t parentDoorId, const glm::vec3& localOffset, DeadLockType type) {
    m_localOffset = localOffset;
    m_type = type;
    m_parentDoorId = parentDoorId;
    
    if (m_type == DeadLockType::BOLT) {
        std::vector<MeshNodeCreateInfo> meshNodeCreateInfoSet;

        MeshNodeCreateInfo& bolt = meshNodeCreateInfoSet.emplace_back();
        bolt.meshName = "Bolt";
        bolt.materialName = "DoorMetals2";
        bolt.blendingMode = BlendingMode::DEFAULT;
        bolt.decalType = DecalType::UNDEFINED;

        MeshNodeCreateInfo& ccatch = meshNodeCreateInfoSet.emplace_back();
        ccatch.meshName = "Catch";
        ccatch.materialName = "DoorMetals2";
        ccatch.blendingMode = BlendingMode::DEFAULT;
        ccatch.decalType = DecalType::UNDEFINED;

        MeshNodeCreateInfo& guide = meshNodeCreateInfoSet.emplace_back();
        guide.meshName = "Guide";
        guide.materialName = "DoorMetals2";
        guide.blendingMode = BlendingMode::DEFAULT;
        guide.decalType = DecalType::UNDEFINED;

        m_meshNodes.Init(NO_ID, "DoorDeadLockBolt", meshNodeCreateInfoSet);
    }
}

void DeadLock::Update(float deltaTime) {
    Door* parentDoor = World::GetDoorByObjectId(m_parentDoorId);
    if (!parentDoor) return;

    const glm::mat4& parentDoorModelMatrix = parentDoor->GetDoorModelMatrix();

  // std::cout << m_parentDoorId << ": " << Util::Mat4ToString(parentDoorModelMatrix) << "\n";

    Transform localTransform;
    localTransform.position = m_localOffset;


    if (MeshNode* meshNode = parentDoor->GetMeshNodes().GetMeshNodeByMeshName("Door_Hinges")) {
        if (Openable* openable = OpenableManager::GetOpenableByOpenableId(meshNode->openableId)) {

           
            glm::mat4 openableMatrix = openable->m_transform.to_mat4();

            //std::cout << m_parentDoorId << ":" << Util::Mat4ToString(openable->m_transform.to_mat4()) << "\n";

            m_meshNodes.Update(parentDoorModelMatrix * openableMatrix * localTransform.to_mat4());

        }




            //m_meshNodes.DrawWorldspaceAABBs(YELLOW);


    }


    m_meshNodes.DrawWorldspaceAABBs(YELLOW);

}


void DeadLock::CleanUp() {
    m_meshNodes.CleanUp();
}