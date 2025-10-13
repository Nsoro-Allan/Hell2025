#include "Drawers.h"
#include "AssetManagement/AssetManager.h"
#include "Managers/OpenableManager.h"
#include "Renderer/Renderer.h"
#include "UniqueID.h"
#include "Util/Util.h"

Drawers::Drawers(uint64_t id, const DrawersCreateInfo& createInfo, const SpawnOffset& spawnOffset) {
    m_createInfo = createInfo;
    m_transform.position = m_createInfo.position + spawnOffset.translation;
    m_transform.rotation = m_createInfo.rotation + glm::vec3(0.0f, spawnOffset.yRotation, 0.0f);
    m_openableIds.clear();
    m_objectId = id;

    if (createInfo.type == DrawersType::LARGE) {
        Model* model = AssetManager::GetModelByName("DrawersLarge2");
        m_meshNodes.InitFromModel(model);
        m_meshNodes.SetObjectTypes(ObjectType::DRAWERS);
        m_meshNodes.SetObjectIds(id);

        m_meshNodes.SetMaterialByMeshName("Key", "T_SmallKey_01a");
        m_meshNodes.SetMaterialByMeshName("DrawersLargeFrame", "T_Main_01a");
        m_meshNodes.SetMaterialByMeshName("DrawersLarge_2nd", "T_Drawers_01a");
        m_meshNodes.SetMaterialByMeshName("DrawersLarge_3rd", "T_Drawers_01a");
        m_meshNodes.SetMaterialByMeshName("DrawersLarge_4th", "T_Drawers_01a");
        m_meshNodes.SetMaterialByMeshName("DrawersLarge_TopL", "T_Drawers_01a");
        m_meshNodes.SetMaterialByMeshName("DrawersLarge_TopR", "T_Drawers_01a");
        m_meshNodes.SetMaterialByMeshName("Handle_TopL", "T_Handles_01a");
        m_meshNodes.SetMaterialByMeshName("Handle_TopR", "T_Handles_01a");
        m_meshNodes.SetMaterialByMeshName("Handle_2nd", "T_Handles_01a");
        m_meshNodes.SetMaterialByMeshName("Handle_3rd", "T_Handles_01a");
        m_meshNodes.SetMaterialByMeshName("Handle_4th", "T_Handles_01a");

        uint64_t drawerTopLeftId = m_openableIds.emplace_back(OpenableManager::CreateOpenable());
        uint64_t drawerTopRightId = m_openableIds.emplace_back(OpenableManager::CreateOpenable());
        uint64_t drawerSecondId = m_openableIds.emplace_back(OpenableManager::CreateOpenable());;
        uint64_t drawerThirdId = m_openableIds.emplace_back(OpenableManager::CreateOpenable());
        uint64_t drawerForthId = m_openableIds.emplace_back(OpenableManager::CreateOpenable());

        m_meshNodes.SetOpenableByMeshName("DrawersLarge_TopL", drawerTopLeftId, id);
        m_meshNodes.SetOpenableByMeshName("DrawersLarge_TopR", drawerTopRightId, id);
        m_meshNodes.SetOpenableByMeshName("DrawersLarge_2nd", drawerSecondId, id);
        m_meshNodes.SetOpenableByMeshName("DrawersLarge_3rd", drawerThirdId, id);
        m_meshNodes.SetOpenableByMeshName("DrawersLarge_4th", drawerForthId, id);
        m_meshNodes.SetOpenableByMeshName("Handle_TopL", drawerTopLeftId, id);
        m_meshNodes.SetOpenableByMeshName("Handle_TopR", drawerTopRightId, id);
        m_meshNodes.SetOpenableByMeshName("Handle_2nd", drawerSecondId, id);
        m_meshNodes.SetOpenableByMeshName("Handle_3rd", drawerThirdId, id);
        m_meshNodes.SetOpenableByMeshName("Handle_4th", drawerForthId, id);

        Openable* openableTopL = OpenableManager::GetOpeneableByOpenableId(drawerTopLeftId);
        openableTopL->openAxis = TRANSLATE_Z;
        openableTopL->currentOpenValue = 0.0f;
        openableTopL->minOpenValue = 0.0f;
        openableTopL->maxOpenValue = 0.158f;
        openableTopL->openSpeed = 1.5f;
        openableTopL->closeSpeed = 1.5f;
        openableTopL->openState = OpenState::CLOSED;
        openableTopL->openingAudio = "DrawerOpen.wav";
        openableTopL->closingAudio = "DrawerClose.wav";
        openableTopL->meshName = "DrawersLarge_TopL";

        Openable* openableTopR = OpenableManager::GetOpeneableByOpenableId(drawerTopRightId);
        openableTopR->openAxis = TRANSLATE_Z;
        openableTopR->currentOpenValue = 0.0f;
        openableTopR->minOpenValue = 0.0f;
        openableTopR->maxOpenValue = 0.155f;
        openableTopR->openSpeed = 1.5f;
        openableTopR->closeSpeed = 1.5f;
        openableTopR->openState = OpenState::CLOSED;
        openableTopR->openingAudio = "DrawerOpen.wav";
        openableTopR->closingAudio = "DrawerClose.wav";
        openableTopR->meshName = "DrawersLarge_TopR";

        Openable* openableSecond = OpenableManager::GetOpeneableByOpenableId(drawerSecondId);
        openableSecond->openAxis = TRANSLATE_Z;
        openableSecond->currentOpenValue = 0.0f;
        openableSecond->minOpenValue = 0.0f;
        openableSecond->maxOpenValue = 0.175f;
        openableSecond->openSpeed = 1.5f;
        openableSecond->closeSpeed = 1.5f;
        openableSecond->openState = OpenState::CLOSED;
        openableSecond->openingAudio = "DrawerOpen.wav";
        openableSecond->closingAudio = "DrawerClose.wav";
        openableSecond->meshName = "DrawersLarge_2nd";

        Openable* openableThird = OpenableManager::GetOpeneableByOpenableId(drawerThirdId);
        openableThird->openAxis = TRANSLATE_Z;
        openableThird->currentOpenValue = 0.0f;
        openableThird->minOpenValue = 0.0f;
        openableThird->maxOpenValue = 0.170f;
        openableThird->openSpeed = 1.5f;
        openableThird->closeSpeed = 1.5f;
        openableThird->openState = OpenState::CLOSED;
        openableThird->openingAudio = "DrawerOpen.wav";
        openableThird->closingAudio = "DrawerClose.wav";
        openableThird->meshName = "DrawersLarge_3rd";

        Openable* openableForth = OpenableManager::GetOpeneableByOpenableId(drawerForthId);
        openableForth->openAxis = TRANSLATE_Z;
        openableForth->currentOpenValue = 0.0f;
        openableForth->minOpenValue = 0.0f;
        openableForth->maxOpenValue = 0.180f;
        openableForth->openSpeed = 1.5f;
        openableForth->closeSpeed = 1.5f;
        openableForth->openState = OpenState::CLOSED;
        openableForth->openingAudio = "DrawerOpen.wav";
        openableForth->closingAudio = "DrawerClose.wav";
        openableForth->meshName = "DrawersLarge_4th";
    }
    else if (createInfo.type == DrawersType::SMALL) {
        Model* model = AssetManager::GetModelByName("DrawersSmall");
        m_meshNodes.InitFromModel(model);
        m_meshNodes.SetMeshMaterials("Drawers");
    }
}

void Drawers::Update(float deltaTime) {
   m_meshNodes.UpdateRenderItems(m_transform.to_mat4());
   m_movedThisFrame = false; // WANRING I AM ONLY TEMPORATYIY HERE

   for (uint64_t openableId : m_openableIds) {
       Openable* openable = OpenableManager::GetOpeneableByOpenableId(openableId);
       if (openable->movedThisFrame) {
           m_movedThisFrame = true;
           break;
       }
   }

   //m_meshNodes.DrawWorldspaceAABB(GREEN);

   glm::vec3 worldSpaceCenter = m_transform.position + GetGizmoOffset();
   Renderer::DrawPoint(worldSpaceCenter, PINK);
}

void Drawers::CleanUp() {

}

void Drawers::SetPosition(const glm::vec3& position) {
    m_createInfo.position = position;
    m_transform.position = position;
}

void Drawers::SetRotation(const glm::vec3& rotation) {
    m_createInfo.rotation = rotation;
    m_transform.rotation = rotation;
};


glm::vec3 Drawers::GetGizmoOffset() {
    glm::vec3 aabbMin = m_meshNodes.m_worldspaceAABB.GetBoundsMin();
    glm::vec3 aabbMax = m_meshNodes.m_worldspaceAABB.GetBoundsMax();
    return ((aabbMin + aabbMax) * glm::vec3(0.5f)) - m_transform.position;
}