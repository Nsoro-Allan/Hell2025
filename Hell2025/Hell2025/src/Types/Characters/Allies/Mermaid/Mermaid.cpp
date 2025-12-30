#include "Mermaid.h"
#include "Audio/Audio.h"
#include "Input/Input.h"
#include "Renderer/Renderer.h"

void Mermaid::Init(MermaidCreateInfo createInfo, SpawnOffset spawnOffset) {
    m_createInfo = createInfo;
    m_spawnOffset = spawnOffset;

    m_transform.position = m_createInfo.position + m_spawnOffset.translation;
    m_transform.rotation = m_createInfo.rotation + glm::vec3(0.0f, m_spawnOffset.yRotation, 0.0f);

    std::vector<MeshNodeCreateInfo> meshNodeCreateInfoSet;

    MeshNodeCreateInfo& rock = meshNodeCreateInfoSet.emplace_back();
    rock.meshName = "Rock";
    rock.rigidDynamic.createObject = true; 
    rock.rigidDynamic.kinematic = true;
    rock.rigidDynamic.offsetTransform = Transform();
    rock.rigidDynamic.filterData.raycastGroup = RAYCAST_DISABLED;
    rock.rigidDynamic.filterData.collisionGroup = CollisionGroup::ENVIROMENT_OBSTACLE;
    rock.rigidDynamic.filterData.collidesWith = (CollisionGroup)(GENERIC_BOUNCEABLE | ITEM_PICK_UP | BULLET_CASING | RAGDOLL_PLAYER | RAGDOLL_ENEMY);   
    rock.rigidDynamic.shapeType = PhysicsShapeType::CONVEX_MESH;
    rock.rigidDynamic.convexMeshModelName = "CollisionMesh_MermaidRock";
    rock.materialName = "Rock";

    m_meshNodes.Init(NO_ID, "Mermaid", meshNodeCreateInfoSet);
    m_meshNodes.SetMaterialByMeshName("Arms", "MermaidArms");
    m_meshNodes.SetMaterialByMeshName("Body", "MermaidBody");
    m_meshNodes.SetMaterialByMeshName("BoobTube", "BoobTube");
    m_meshNodes.SetMaterialByMeshName("EyelashLower", "MermaidLashes");
    m_meshNodes.SetMaterialByMeshName("EyelashUpper", "MermaidLashes");
    m_meshNodes.SetMaterialByMeshName("EyeLeft", "MermaidEye");
    m_meshNodes.SetMaterialByMeshName("EyeRight", "MermaidEye");
    m_meshNodes.SetMaterialByMeshName("Face", "MermaidFace");
    m_meshNodes.SetMaterialByMeshName("HairInner", "MermaidHair");
    m_meshNodes.SetMaterialByMeshName("HairOutta", "MermaidHair");
    m_meshNodes.SetMaterialByMeshName("HairScalp", "MermaidScalp");
    m_meshNodes.SetMaterialByMeshName("Nails", "Nails");
    //m_meshNodes.SetMaterialByMeshName("Rock", "Rock");
    m_meshNodes.SetMaterialByMeshName("Tail", "MermaidTail");
    m_meshNodes.SetMaterialByMeshName("TailFin", "MermaidTail");

    m_meshNodes.SetBlendingModeByMeshName("EyelashLower", BlendingMode::BLENDED);
    m_meshNodes.SetBlendingModeByMeshName("EyelashUpper", BlendingMode::BLENDED);
    m_meshNodes.SetBlendingModeByMeshName("HairScalp", BlendingMode::BLENDED);
    m_meshNodes.SetBlendingModeByMeshName("HairOutta", BlendingMode::HAIR_TOP_LAYER);
    m_meshNodes.SetBlendingModeByMeshName("HairInner", BlendingMode::HAIR_UNDER_LAYER);
}

void Mermaid::Update(float deltaTime) {
    UpdateRenderItems();

    static bool titties = false;
    if (Input::KeyPressed(HELL_KEY_I)) {
        titties = !titties;
        if (titties) {
            m_meshNodes.SetBlendingModeByMeshName("BoobTube", BlendingMode::DO_NOT_RENDER);
        }
        else {
            m_meshNodes.SetBlendingModeByMeshName("BoobTube", BlendingMode::DEFAULT);
        }
        Audio::PlayAudio(AUDIO_SELECT, 1.00f);
    }

    m_worldForward = m_transform.to_mat4() * glm::vec4(m_localForward, 0.0f);

    //DebugDraw();
}

void Mermaid::DebugDraw() {
    glm::vec3 p1 = m_transform.position;
    glm::vec3 p2 = m_transform.position + m_worldForward;
    Renderer::DrawPoint(p1, YELLOW);
    Renderer::DrawPoint(p2, YELLOW);
    Renderer::DrawLine(p1, p2, YELLOW);
}

void Mermaid::UpdateRenderItems() {
    m_meshNodes.Update(m_transform.to_mat4());
}

void Mermaid::CleanUp() {
    // Nothing as of yet
}