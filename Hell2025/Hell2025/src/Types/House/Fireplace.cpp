#include "Fireplace.h"
#include "Bible/Bible.h"
#include "Input/Input.h"
#include "Renderer/Renderer.h"
#include "Util/Util.h"

Fireplace::Fireplace(uint64_t id, const FireplaceCreateInfo& createInfo, const SpawnOffset& spawnOffset) {
    m_id = id;
    m_createInfo = createInfo;
    m_transform.position = m_createInfo.position + spawnOffset.translation;
    m_transform.rotation.y = m_createInfo.yEulerRotation + spawnOffset.yRotation;

    //m_transform.rotation.y = HELL_PI;

    UpdateWorldMatrix();

    //m_createInfo.type = FireplaceType::WOOD_STOVE;

    std::vector<MeshNodeCreateInfo> meshNodeCreateInfoSet;

    if (m_createInfo.type == FireplaceType::DEFAULT) {
        m_meshNodes.Init(id, "Fireplace", meshNodeCreateInfoSet);

        m_meshNodes.SetMeshMaterials("NumGrid");
        m_meshNodes.SetMeshMaterials("Ceiling2");
        m_meshNodes.SetMeshMaterials("WallPaper");
        m_meshNodes.SetMeshMaterials("PlasterRed");
        m_meshNodes.SetMeshMaterials("T_Walls");

        m_meshNodes.SetMaterialByMeshName("Fireplace_Wall_Damage", "Fireplace");
        m_meshNodes.SetMaterialByMeshName("Fireplace_Wall_Damage.001", "Fireplace");
        m_meshNodes.SetMaterialByMeshName("Fireplace_Bricks_low", "Fireplace");
        m_meshNodes.SetMaterialByMeshName("Fireplace_Cap_low", "Fireplace");
        m_meshNodes.SetMaterialByMeshName("Fireplace_Log_Tray_low", "Fireplace");
        m_meshNodes.SetMaterialByMeshName("Fireplace_Log_Try_Holder_low", "Fireplace");
        m_meshNodes.SetMaterialByMeshName("Fireplace_Floor_low", "Fireplace");

        m_meshNodes.SetMaterialByMeshName("Fireplace_Mantle_low", "Fireplace");
        m_meshNodes.SetMaterialByMeshName("Fireplace_Mantle_Trim_low", "Fireplace");

        m_meshNodes.SetMaterialByMeshName("Fireplace_Wood_Log1_low", "T_Firewood");
        m_meshNodes.SetMaterialByMeshName("Fireplace_Wood_Log2_low", "T_Firewood");
        m_meshNodes.SetMaterialByMeshName("Fireplace_Wood_Log3_low", "T_Firewood");
        m_meshNodes.SetMaterialByMeshName("Fireplace_Wood_Log4_low", "T_Firewood");
        m_meshNodes.SetMaterialByMeshName("Fireplace_Wood_Log5_low", "T_Firewood");
        m_meshNodes.SetMaterialByMeshName("Fireplace_Coal1_low", "T_Firewood");
        m_meshNodes.SetMaterialByMeshName("Fireplace_Coal2_low", "T_Firewood");

        m_meshNodes.SetMaterialByMeshName("Fireplace_Walls_Chipped", "T_WallsChippedEdges");
        m_meshNodes.SetBlendingModeByMeshName("Fireplace_Walls_Chipped", BlendingMode::ALPHA_DISCARD);
    }

	if (m_createInfo.type == FireplaceType::WOOD_STOVE) {

		MeshNodeCreateInfo& door = meshNodeCreateInfoSet.emplace_back();
		door.meshName = "FireplaceBrick_StoveDoor";
        door.materialName = "T_Stove";
        door.openable.isOpenable = true;
		door.openable.openAxis = OpenAxis::ROTATE_Y_NEG;
		door.openable.initialOpenState = OpenState::CLOSED;
		door.openable.minOpenValue = 0.0f;
		door.openable.maxOpenValue = 1.7;
		door.openable.openSpeed = 7.25f;
		door.openable.closeSpeed = 7.25f;
		//door.openable.openingAudio = "BathroomCabinetOpen.wav";
		//door.openable.closingAudio = "BathroomCabinetClose.wav";
		door.openable.additionalTriggerMeshNames = { "FireplaceBrick_StoveHandle", "FireplaceBrick_StoveGlass"};

		m_meshNodes.Init(id, "FireplaceBrick", meshNodeCreateInfoSet);


		m_meshNodes.SetMaterialByMeshName("FireplaceBrick_Floor", "T_Floor");
		m_meshNodes.SetMaterialByMeshName("FireplaceBrick_Stove", "T_Stove");
		m_meshNodes.SetMaterialByMeshName("FireplaceBrick_StoveDoor", "T_Stove");
		m_meshNodes.SetMaterialByMeshName("FireplaceBrick_StoveHandle", "T_Stove");
		m_meshNodes.SetMaterialByMeshName("FireplaceBrick_StoveWindSlide", "T_Stove");
		m_meshNodes.SetMaterialByMeshName("FireplaceBrick_StoveGlass", "T_GlassStove");
		m_meshNodes.SetMaterialByMeshName("FireplaceBrick_Tool0", "T_Tools");
		m_meshNodes.SetMaterialByMeshName("FireplaceBrick_Tool1", "T_Tools");
		m_meshNodes.SetMaterialByMeshName("FireplaceBrick_Tool2", "T_Tools");
		m_meshNodes.SetMaterialByMeshName("FireplaceBrick_WallFront", "T_BrickWall2");
		m_meshNodes.SetMaterialByMeshName("FireplaceBrick_WallMain", "T_BrickWall1");
		m_meshNodes.SetMaterialByMeshName("FireplaceBrick_WallExtended", "T_BrickWall1");

		m_meshNodes.SetBlendingModeByMeshName("FireplaceBrick_StoveGlass", BlendingMode::GLASS);
	}

    m_wallWidth = 0.766488f * 2.0f;
    m_wallDepth = 0.425;
    m_wallDepth = 0.450083f;

    glm::vec3 boundsMin = glm::vec3(-0.1f, 0.0f, -m_wallWidth * 0.5f);
    glm::vec3 boundsMax = glm::vec3(m_wallDepth, 2.7f, m_wallWidth * 0.5f);
    m_wallsAabb = AABB(boundsMin, boundsMax);
}

void Fireplace::UpdateWorldMatrix() {
    m_worldMatrix = m_transform.to_mat4();

    glm::vec3 localForward = glm::vec3(1.0, 0.0, 0.0f);
    glm::vec3 localRight = glm::vec3(0.0, 0.0, 1.0f);

    m_worldForward = m_worldMatrix * glm::vec4(localForward, 0.0f);
    m_worldRight = m_worldMatrix * glm::vec4(localRight, 0.0f);
}

void Fireplace::SetPosition(const glm::vec3& position) {
    m_createInfo.position = position;
    m_transform.position = position;
    UpdateWorldMatrix();
}

void Fireplace::SetYEulerRotation(float rotation) {
    m_createInfo.yEulerRotation = rotation;
    m_transform.rotation.y = rotation;
    UpdateWorldMatrix();
}


void Fireplace::Update(float deltaTime) {
    m_meshNodes.Update(m_worldMatrix);

    //Renderer::DrawAABB(m_wallsAabb, WHITE, m_worldMatrix);

    //glm::vec3 center = m_worldMatrix[3];
    //glm::vec3 forwardPoint = center + m_worldForward;
    //glm::vec3 rightPoint = center + m_worldRight;
    //
    //Renderer::DrawLine(center, forwardPoint, BLUE);
    //Renderer::DrawLine(center, rightPoint, YELLOW);

}

void Fireplace::CleanUp() {

}