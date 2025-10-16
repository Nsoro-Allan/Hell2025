#include "GenericObject.h"
#include "AssetManagement/AssetManager.h"
#include "Bible/Bible.h"
#include "HellLogging.h"
#include "Managers/OpenableManager.h"
#include "Renderer/Renderer.h"
#include "UniqueID.h"
#include "Util/Util.h"

GenericObject::GenericObject(uint64_t id, const GenericObjectCreateInfo& createInfo, const SpawnOffset& spawnOffset) {
    m_createInfo = createInfo;
    m_transform.position = m_createInfo.position + spawnOffset.translation;
    m_transform.rotation = m_createInfo.rotation + glm::vec3(0.0f, spawnOffset.yRotation, 0.0f);
    m_transform.scale = m_createInfo.scale;
    m_objectId = id;

    //Bible::ConfigureMeshNodes(id, m_createInfo.type, m_meshNodes);

    if (m_createInfo.type == GenericObjectType::DRAWERS_LARGE) {

        std::vector<MeshNodeCreateInfo> meshNodeCreateInfoSet;

        MeshNodeCreateInfo& largeFrame = meshNodeCreateInfoSet.emplace_back();
        largeFrame.meshName = "DrawersLargeFrame";
        largeFrame.materialName = "T_Main_01a";

        MeshNodeCreateInfo& drawerTopL = meshNodeCreateInfoSet.emplace_back();
        drawerTopL.type = MeshNodeType::OPENABLE;
        drawerTopL.materialName = "T_Drawers_01a";
        drawerTopL.meshName = "DrawersLarge_TopL";
        drawerTopL.openable.additionalTriggerMeshNames = { "Handle_TopL" };
        drawerTopL.openable.openAxis = OpenAxis::TRANSLATE_Z;
        drawerTopL.openable.initialOpenState = OpenState::CLOSED;
        drawerTopL.openable.initialOpenValue = 0.0f;
        drawerTopL.openable.minOpenValue = 0.0f;
        drawerTopL.openable.maxOpenValue = 0.158f;
        drawerTopL.openable.openSpeed = 1.5f;
        drawerTopL.openable.closeSpeed = 1.5f;
        drawerTopL.openable.openingAudio = "DrawerOpen.wav";
        drawerTopL.openable.closingAudio = "DrawerClose.wav";
        drawerTopL.openable.lockedAudio = "Locked.wav";

        MeshNodeCreateInfo& drawerTopR = meshNodeCreateInfoSet.emplace_back();
        drawerTopR.type = MeshNodeType::OPENABLE;
        drawerTopR.materialName = "T_Drawers_01a";
        drawerTopR.meshName = "DrawersLarge_TopR";
        drawerTopR.openable.additionalTriggerMeshNames = { "Handle_TopR" };
        drawerTopR.openable.openAxis = OpenAxis::TRANSLATE_Z;
        drawerTopR.openable.initialOpenState = OpenState::CLOSED;
        drawerTopR.openable.initialOpenValue = 0.0f;
        drawerTopR.openable.minOpenValue = 0.0f;
        drawerTopR.openable.maxOpenValue = 0.155f;
        drawerTopR.openable.openSpeed = 1.5f;
        drawerTopR.openable.closeSpeed = 1.5f;
        drawerTopR.openable.openingAudio = "DrawerOpen.wav";
        drawerTopR.openable.closingAudio = "DrawerClose.wav";
        drawerTopR.openable.lockedAudio = "Locked.wav";

        m_meshNodes.Init(id, "DrawersLarge", meshNodeCreateInfoSet);

        m_meshNodes.SetMaterialByMeshName("DrawersLarge_2nd", "T_Drawers_01a");
        m_meshNodes.SetMaterialByMeshName("DrawersLarge_3rd", "T_Drawers_01a");
        m_meshNodes.SetMaterialByMeshName("DrawersLarge_4th", "T_Drawers_01a");
        m_meshNodes.SetMaterialByMeshName("Handle_TopL", "T_Handles_01a");
        m_meshNodes.SetMaterialByMeshName("Handle_TopR", "T_Handles_01a");
        m_meshNodes.SetMaterialByMeshName("Handle_2nd", "T_Handles_01a");
        m_meshNodes.SetMaterialByMeshName("Handle_3rd", "T_Handles_01a");
        m_meshNodes.SetMaterialByMeshName("Handle_4th", "T_Handles_01a");

        OpenableCreateInfo openableCreateInfo;
        openableCreateInfo.openAxis = OpenAxis::TRANSLATE_Z;
        openableCreateInfo.initialOpenState = OpenState::CLOSED;
        openableCreateInfo.initialOpenValue = 0.0f;
        openableCreateInfo.minOpenValue = 0.0f;
        openableCreateInfo.maxOpenValue = 0.158f;
        openableCreateInfo.openSpeed = 1.5f;
        openableCreateInfo.closeSpeed = 1.5f;
        openableCreateInfo.openingAudio = "DrawerOpen.wav";
        openableCreateInfo.closingAudio = "DrawerClose.wav";
        openableCreateInfo.lockedAudio = "Locked.wav";
        //m_meshNodes.CreateOpenable("DrawersLarge_TopL", openableCreateInfo);

        //uint64_t drawerTopLeftOpenableId = m_openableIds.emplace_back(OpenableManager::CreateOpenable(openableCreateInfo));

        openableCreateInfo.maxOpenValue = 0.155f;
        //openableCreateInfo.meshName = "DrawersLarge_TopR";
        //uint64_t drawerTopRightOpenableId = m_openableIds.emplace_back(OpenableManager::CreateOpenable(openableCreateInfo));

        openableCreateInfo.maxOpenValue = 0.175f;
        //openableCreateInfo.meshName = "DrawersLarge_2nd";
        //uint64_t drawerSecondOpenableId = m_openableIds.emplace_back(OpenableManager::CreateOpenable(openableCreateInfo));;

        openableCreateInfo.maxOpenValue = 0.170f;
        //openableCreateInfo.meshName = "DrawersLarge_3rd";
        //uint64_t drawerThirdOpenableId = m_openableIds.emplace_back(OpenableManager::CreateOpenable(openableCreateInfo));

        openableCreateInfo.maxOpenValue = 0.180f;
        //openableCreateInfo.meshName = "DrawersLarge_4th";
        //uint64_t drawerForthOpenableId = m_openableIds.emplace_back(OpenableManager::CreateOpenable(openableCreateInfo));

        //m_meshNodes.SetOpenableByMeshName("DrawersLarge_TopL", drawerTopLeftOpenableId, id);
        //m_meshNodes.SetOpenableByMeshName("DrawersLarge_TopR", drawerTopRightOpenableId, id);
        //m_meshNodes.SetOpenableByMeshName("DrawersLarge_2nd", drawerSecondOpenableId, id);
        //m_meshNodes.SetOpenableByMeshName("DrawersLarge_3rd", drawerThirdOpenableId, id);
        //m_meshNodes.SetOpenableByMeshName("DrawersLarge_4th", drawerForthOpenableId, id);
        ////m_meshNodes.SetOpenableByMeshName("Handle_TopL", drawerTopLeftOpenableId, id);
        ////m_meshNodes.SetOpenableByMeshName("Handle_TopR", drawerTopRightOpenableId, id);
        //m_meshNodes.SetOpenableByMeshName("Handle_2nd", drawerSecondOpenableId, id);
        //m_meshNodes.SetOpenableByMeshName("Handle_3rd", drawerThirdOpenableId, id);
        //m_meshNodes.SetOpenableByMeshName("Handle_4th", drawerForthOpenableId, id);
    }
    else {
        Logging::Error() << "GenericObject constructor failed because you haven't implemented this GenericObjectType";
    }
}

void GenericObject::Update(float deltaTime) {
   m_meshNodes.UpdateRenderItems(m_transform.to_mat4());
   Renderer::DrawPoint(m_transform.position, PINK);
}

void GenericObject::CleanUp() {
    m_meshNodes.CleanUp();
}

void GenericObject::SetPosition(const glm::vec3& position) {
    m_createInfo.position = position;
    m_transform.position = position;
}

void GenericObject::SetRotation(const glm::vec3& rotation) {
    m_createInfo.rotation = rotation;
    m_transform.rotation = rotation;
};

glm::vec3 GenericObject::GetGizmoOffset() {
    return glm::vec3(0.0f);
    //glm::vec3 aabbMin = m_meshNodes.m_worldspaceAABB.GetBoundsMin();
    //glm::vec3 aabbMax = m_meshNodes.m_worldspaceAABB.GetBoundsMax();
    //return ((aabbMin + aabbMax) * glm::vec3(0.5f)) - m_transform.position;
}