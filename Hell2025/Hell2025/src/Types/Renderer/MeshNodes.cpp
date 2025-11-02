#include "MeshNodes.h"
#include "AssetManagement/AssetManager.h"
#include "HellLogging.h"
#include "Input/Input.h"
#include "Managers/MirrorManager.h"
#include "Managers/OpenableManager.h"
#include "Renderer/RenderDataManager.h"
#include "Renderer/Renderer.h"
#include "World/World.h"
#include "Physics/Physics.h"
#include "UniqueID.h"
#include "Util.h"

void MeshNodes::Init(uint64_t parentId, const std::string& modelName, const std::vector<MeshNodeCreateInfo>& meshNodeCreateInfoSet) {
    Model* model = AssetManager::GetModelByName(modelName);
    if (!model) {
        Logging::Error() << "MeshNodes::Init() failed because modelName was not found";
        return;
    }

    CleanUp();

    m_modelName = modelName;
    m_nodeCount = model->GetMeshCount();
    m_meshNodes.resize(m_nodeCount);

    // Defaults
    for (size_t i = 0; i < m_nodeCount; i++) {
        int globalMeshIndex = model->GetMeshIndices()[i];

        Mesh* mesh = AssetManager::GetMeshByIndex(globalMeshIndex);
        if (!mesh) {
            Logging::Error() << "MeshNodes::Init(...) failed to find mesh by global index " << i;
            continue;
        }

        // Map mesh name to local index for easy lookup
        m_localIndexMap[mesh->GetName()] = i;

        MeshNode& meshNode = m_meshNodes[i];
        meshNode.blendingMode = BlendingMode::BLEND_DISABLED;
        meshNode.materialIndex = AssetManager::GetMaterialIndexByName(DEFAULT_MATERIAL_NAME);
        meshNode.transform = Transform();
        meshNode.transformPreviousFrame = Transform(glm::vec3(666.0f)); // Forces dirty
        meshNode.localParentIndex = mesh->parentIndex;
        meshNode.localTransform = mesh->localTransform;
        meshNode.inverseBindTransform = mesh->inverseBindTransform;
        meshNode.localModelMatrix = glm::mat4(1.0f);
        meshNode.worldspaceAabb = AABB();
        meshNode.parentObjectId = parentId;
        meshNode.type = MeshNodeType::DEFAULT;
        meshNode.globalMeshIndex = globalMeshIndex;
        meshNode.customId = 0;
        meshNode.nodeIndex = i;
    }

    // If the model contains armatures, store the first one (TODO: allow more maybe)
    if (model->m_modelData.armatures.size() > 0) {
        m_armatureData = model->m_modelData.armatures[0];
    }

    // Apply any mesh node create info
    for (const MeshNodeCreateInfo& createInfo : meshNodeCreateInfoSet) {

        MeshNode* meshNode = GetMeshNodeByMeshName(createInfo.meshName);
        if (!meshNode) {
            Logging::Error() << "MeshNodes::Init(...) failed to process meshNodeCreateInfoSet, mesh name '" << createInfo.meshName << "' not found in model '" << modelName << "'";
            continue;
        }
        meshNode->materialIndex = AssetManager::GetMaterialIndexByName(createInfo.materialName);
        meshNode->blendingMode = createInfo.blendingMode;
        meshNode->isGold = createInfo.isGold;
        meshNode->type = createInfo.type;
        meshNode->customId = createInfo.customId;

        int nodeIndex = m_localIndexMap[createInfo.meshName];

        if (meshNode->type == MeshNodeType::OPENABLE) {
            uint32_t openableId = OpenableManager::CreateOpenable(createInfo.openable, parentId);
            meshNode->openableId = openableId;

            for (const std::string& triggerMeshName : createInfo.openable.additionalTriggerMeshNames) {
                if (MeshNode* triggerMeshNode = GetMeshNodeByMeshName(triggerMeshName)) {
                    switch (triggerMeshNode->type) {
                        case MeshNodeType::OPENABLE:        Logging::Error() << "MeshNodes::Init(...) failed to set '" << createInfo.meshName << "' as a Openable trigger node because it already has an Openable"; break;
                        case MeshNodeType::RIGID_DYNAMIC:   Logging::Error() << "MeshNodes::Init(...) failed to set '" << createInfo.meshName << "' as a trigger node because it of type RIGID_DYNAMIC"; break;
                        case MeshNodeType::RIGID_STATIC:    Logging::Error() << "MeshNodes::Init(...) failed to set '" << createInfo.meshName << "' as a trigger node because it of type RIGID_STATIC"; break;
                        default:                            triggerMeshNode->openableId = openableId; break;
                    }
                }
            }
        }

        // If blending mode is mirror, then add a new mirror to the world
        if (meshNode->blendingMode == BlendingMode::MIRROR) {
            MirrorManager::AddMirror(parentId, meshNode->nodeIndex, meshNode->globalMeshIndex);
        }

        if (meshNode->type == MeshNodeType::RIGID_DYNAMIC) {
            // Break connection with parent
            meshNode->localParentIndex = -1;

            // If you wanna add physics to MeshNodes that don't belong to a GenericObject then you are gonna need a bit of a different plan here or a giant lists of ifs, ugly...
            Transform initalTransform; 
            if (GenericObject* parent = World::GetGenericObjectById(parentId)) {
                initalTransform.position = parent->GetPosition();
                initalTransform.rotation = parent->GetRotation();
                Logging::ToDo() << "FOUND PARENT !!!!!!!!!!!!!!!!!!!!!!!!!!!" << initalTransform.position << " " << initalTransform.rotation;
            }
            else {
                Logging::Error() << "Did not find parent!!!!!!!!!!!!!!!!!!!!!!!!!!!";
            }

            PhysicsFilterData filterData = createInfo.rigidDynamic.filterData;
            float mass = createInfo.rigidDynamic.mass;

            // Test box
            glm::vec3 boxExtents = glm::vec3(0.5f);
            meshNode->physicsId = Physics::CreateRigidDynamicFromBoxExtents(initalTransform, boxExtents, mass, filterData);
        }
    }
}

void MeshNodes::PrintMeshNames() {
    std::cout << m_modelName << "\n";

    for (size_t i = 0; i < m_meshNodes.size(); i++) {
        MeshNode& meshNode = m_meshNodes[i];
        Mesh* mesh = AssetManager::GetMeshByIndex(meshNode.globalMeshIndex);
        if (!mesh) continue;
    
        std::cout << "-" << i << ": " << mesh->GetName() << "\n";
    }
}

void MeshNodes::SetGoldFlag(bool flag) {
    m_isGold = flag;
}

bool MeshNodes::NodeExists(const std::string& meshName) {
    const auto it = m_localIndexMap.find(meshName);
    return (it != m_localIndexMap.end());
}

bool MeshNodes::BoneExists(const std::string& boneName) {
    for (Bone& bone : m_armatureData.bones) {
        if (bone.name == boneName)
            return true;
    }
    return false;
}

bool MeshNodes::HasNodeWithObjectId(uint64_t objectId) const {
    for (const MeshNode& meshNode : m_meshNodes) {
        if (meshNode.parentObjectId == objectId) {
            return true;
        }
    }
    return false;
}

void MeshNodes::CleanUp() {
    m_modelName = "";
    m_nodeCount = 0;
    m_meshNodes.clear();
    m_localIndexMap.clear();
    m_renderItems.clear();
}

void MeshNodes::SetTransformByMeshName(const std::string& meshName, Transform transform) {
    MeshNode* meshNode = GetMeshNodeByMeshName(meshName);
    if (meshNode) {
        meshNode->transform = transform;
    }
}

void MeshNodes::SetMeshMaterials(const std::string& materialName) {
    int materialIndex = AssetManager::GetMaterialIndexByName(materialName);
    if (materialIndex == -1) {
        Logging::Error() << "MeshNodes::SetMeshMaterials() failed: '" << materialName << "' not found";
        return;
    }
    for (MeshNode& meshNode : m_meshNodes) {
        meshNode.materialIndex = materialIndex;
    }
}

void MeshNodes::SetMaterialByMeshName(const std::string& meshName, const std::string& materialName) {
    int materialIndex = AssetManager::GetMaterialIndexByName(materialName);
    if (materialIndex == -1) {
        Logging::Error() << "MeshNodes::SetMaterialByMeshName() failed: '" << materialName << "' not found";
        return;
    }
    MeshNode* meshNode = GetMeshNodeByMeshName(meshName);
    if (meshNode) {
        meshNode->materialIndex = materialIndex;
    }
}

void MeshNodes::SetBlendingModeByMeshName(const std::string& meshName, BlendingMode blendingMode) {
    MeshNode* meshNode = GetMeshNodeByMeshName(meshName);
    if (meshNode) {
        meshNode->blendingMode = blendingMode;
    }
}

void MeshNodes::SetObjectIdByMeshName(const std::string& meshName, uint64_t id) {
    MeshNode* meshNode = GetMeshNodeByMeshName(meshName);
    if (meshNode) {
        meshNode->parentObjectId = id;
    }
}

void MeshNodes::SetOpenableByMeshName(const std::string& meshName, uint64_t openableId, uint64_t parentObjectId) {
    Openable* openable = OpenableManager::GetOpenableByOpenableId(openableId);
    if (!openable) {
        Logging::Error() << "MeshNodes::SetOpenableByMeshName failed: openableId " << openableId << " not found\n";
        return;
    }

    // A little messy. Maybe rethink me??? It overwrites objectId
    MeshNode* meshNode = GetMeshNodeByMeshName(meshName);
    if (meshNode) {
        openable->SetParentObjectId(meshNode->parentObjectId);
        meshNode->parentObjectId = openableId;
    }
    else {
        Logging::Error() << "MeshNodes::SetOpenableByMeshName failed because meshName '" << meshName << "' not found\n";
    }
}

void MeshNodes::DrawWorldspaceAABB(glm::vec4 color) {
    Renderer::DrawAABB(m_worldspaceAABB, color);
}

void MeshNodes::DrawWorldspaceAABBs(glm::vec4 color) {
    for (MeshNode& meshNode : m_meshNodes) {
        Renderer::DrawAABB(meshNode.worldspaceAabb, color);
    }
}

void MeshNodes::ForceDirty() {
    m_forceDirty = true;
}

const AABB* MeshNodes::GetWorldSpaceAabbByMeshName(const std::string& meshName) {
    const static AABB invalidAabb;

    MeshNode* meshNode = GetMeshNodeByMeshName(meshName);
    if (meshNode) {
        return &meshNode->worldspaceAabb;
    }

    Logging::Error() << "MeshNodes::GetWorldSpaceAabbByMeshName failed because meshName '" << meshName << "' not found\n";
    return &invalidAabb;
}

int32_t MeshNodes::GetGlobalMeshIndex(int nodeIndex) {
    MeshNode* meshNode = GetMeshNodeByLocalIndex(nodeIndex);
    if (meshNode) {
        return meshNode->globalMeshIndex;
    }
    Logging::Error() << "MeshNodes::GetGlobalMeshIndex failed because node index '" << nodeIndex << "' not found\n";
    return -1;
}

Material* MeshNodes::GetMaterial(int nodeIndex) {
    MeshNode* meshNode = GetMeshNodeByLocalIndex(nodeIndex);
    if (!meshNode) return AssetManager::GetDefaultMaterial();

    return AssetManager::GetMaterialByIndex(meshNode->materialIndex);
}

void MeshNodes::UpdateHierarchy() {
    for (MeshNode& meshNode : m_meshNodes) {
        MeshNode* parentMeshNode = GetMeshNodeByLocalIndex(meshNode.localParentIndex);
        if (parentMeshNode) {
            meshNode.localModelMatrix = parentMeshNode->localModelMatrix * meshNode.localTransform * meshNode.transform.to_mat4();
        }
        else {
            meshNode.localModelMatrix = meshNode.localTransform * meshNode.transform.to_mat4();
        }
    }
}

void MeshNodes::UpdateRenderItems(const glm::mat4& worldMatrix) {
    // Check if the world matrix changed this frame
    const bool worldMatrixDirty = (!Util::Mat4NearlyEqual(worldMatrix, m_worldMatrixPreviousFrame));

    // Is the hierarchy dirty?
    bool hierarchyDirty = false;

    if (m_forceDirty) {
        hierarchyDirty = true;
    }

    // Openables
    for (MeshNode& meshNode : m_meshNodes) {
        if (meshNode.type == MeshNodeType::OPENABLE) {
            if (Openable* openable = OpenableManager::GetOpenableByOpenableId(meshNode.openableId)) {
                if (openable->IsDirty()) {
                    meshNode.transform = openable->m_transform;
                    hierarchyDirty = true;   
                }
            }
        }
    }

    for (MeshNode& meshNode : m_meshNodes) {
        if (meshNode.type == MeshNodeType::RIGID_DYNAMIC) {
            if (RigidDynamic* rigidDynamic = Physics::GetRigidDynamicById(meshNode.physicsId)) {
                Renderer::DrawPoint(rigidDynamic->GetCurrentPosition(), YELLOW);
            }
        }
    }

    if (hierarchyDirty) {
        UpdateHierarchy();
    }

    // If neither are dirty, then no need to recompute RenderItems
    if (!worldMatrixDirty && !hierarchyDirty) {
        m_isDirty = false;
        return;
    }
    m_isDirty = worldMatrixDirty || hierarchyDirty;

    // If you made it this far, then AABBs are now also dirty
    UpdateAABBs(worldMatrix);

    m_renderItems.clear();
    m_renderItemsAlphaDiscarded.clear();
    m_renderItemsBlended.clear();
    m_renderItemsGlass.clear();
    m_renderItemsHairTopLayer.clear();
    m_renderItemsHairBottomLayer.clear();
    m_renderItemsToiletWater.clear();
    m_renderItemsMirror.clear();

    for (size_t i = 0; i < m_meshNodes.size(); i++) {
        MeshNode& meshNode = m_meshNodes[i];
        Material* material = GetMaterial(i);
        if (!material) continue;

        meshNode.worldModelMatrix = worldMatrix * meshNode.localModelMatrix;

        meshNode.renderItem.objectType = (int)UniqueID::GetType(meshNode.parentObjectId);
        meshNode.renderItem.openableId = meshNode.openableId;
        meshNode.renderItem.customId = meshNode.customId;
        meshNode.renderItem.modelMatrix = meshNode.worldModelMatrix;
        meshNode.renderItem.inverseModelMatrix = glm::inverse(meshNode.renderItem.modelMatrix);
        meshNode.renderItem.meshIndex = GetGlobalMeshIndex(i);
        meshNode.renderItem.baseColorTextureIndex = material->m_basecolor;
        meshNode.renderItem.normalMapTextureIndex = material->m_normal;
        meshNode.renderItem.rmaTextureIndex = material->m_rma;
        meshNode.renderItem.aabbMin = glm::vec4(meshNode.worldspaceAabb.GetBoundsMin(), 0.0f);
        meshNode.renderItem.aabbMax = glm::vec4(meshNode.worldspaceAabb.GetBoundsMax(), 0.0f);

        // If this object is gold, replace basecolor and rma textures with that of gold material, normal map does not change
        if (m_isGold) {
            static Material* goldMaterial = AssetManager::GetMaterialByName("Gold");
            meshNode.renderItem.baseColorTextureIndex = goldMaterial->m_basecolor;
            meshNode.renderItem.rmaTextureIndex = goldMaterial->m_rma;
        }

        Util::PackUint64(meshNode.parentObjectId, meshNode.renderItem.objectIdLowerBit, meshNode.renderItem.objectIdUpperBit);

        if (meshNode.blendingMode == BlendingMode::MIRROR) {
            static bool test = true;
            if (Input::KeyPressed(HELL_KEY_M)) {
                test = !test;
            }

            if (test) {
                m_renderItemsMirror.push_back(meshNode.renderItem);
                m_renderItemsGlass.push_back(meshNode.renderItem);
            }
            else {
                m_renderItems.push_back(meshNode.renderItem);
            }
        }

        switch (meshNode.blendingMode) {
            case BlendingMode::BLEND_DISABLED:    m_renderItems.push_back(meshNode.renderItem);                 break;
            case BlendingMode::ALPHA_DISCARDED:   m_renderItemsAlphaDiscarded.push_back(meshNode.renderItem);   break;
            case BlendingMode::BLENDED:           m_renderItemsBlended.push_back(meshNode.renderItem);          break;
            case BlendingMode::GLASS:             m_renderItemsGlass.push_back(meshNode.renderItem);            break;
            case BlendingMode::HAIR_TOP_LAYER:    m_renderItemsHairTopLayer.push_back(meshNode.renderItem);     break;
            case BlendingMode::HAIR_UNDER_LAYER:  m_renderItemsHairBottomLayer.push_back(meshNode.renderItem);  break;
            case BlendingMode::TOILET_WATER:      m_renderItemsToiletWater.push_back(meshNode.renderItem);      break;
            default: break;
        }
    }

    // Store previous frame data
    for (MeshNode& meshNode : m_meshNodes) {
        meshNode.transformPreviousFrame = meshNode.transform;
    }
    m_worldMatrixPreviousFrame = worldMatrix;
    m_forceDirty = false;
}

void MeshNodes::UpdateAABBs(const glm::mat4& worldMatrix) {
    glm::vec3 min(std::numeric_limits<float>::max());
    glm::vec3 max(-std::numeric_limits<float>::max());
    bool found = false;

    for (MeshNode& meshNode : m_meshNodes) {
        Mesh* mesh = AssetManager::GetMeshByIndex(meshNode.globalMeshIndex);
        if (!mesh) continue;

        glm::vec3 localMin = mesh->aabbMin;
        glm::vec3 localMax = mesh->aabbMax;
        glm::vec3 c = 0.5f * (localMin + localMax);
        glm::vec3 e = 0.5f * (localMax - localMin);

        glm::mat4 M = worldMatrix * meshNode.localModelMatrix;

        glm::vec3 worldCenter = glm::vec3(M * glm::vec4(c, 1.0f));

        glm::vec3 col0 = glm::vec3(M[0]);
        glm::vec3 col1 = glm::vec3(M[1]);
        glm::vec3 col2 = glm::vec3(M[2]);

        glm::vec3 worldExtents =
            glm::abs(col0) * e.x +
            glm::abs(col1) * e.y +
            glm::abs(col2) * e.z;

        glm::vec3 worldMin = worldCenter - worldExtents;
        glm::vec3 worldMax = worldCenter + worldExtents;

        meshNode.worldspaceAabb = AABB(worldMin, worldMax);

        min = glm::min(min, worldMin);
        max = glm::max(max, worldMax);
        found = true;
    }

    m_worldspaceAABB = found ? AABB(min, max) : AABB();
}

const void MeshNodes::SubmitRenderItems() const {
    RenderDataManager::SubmitRenderItems(m_renderItems);
    RenderDataManager::SubmitRenderItemsAlphaDiscard(m_renderItemsAlphaDiscarded);
    RenderDataManager::SubmitRenderItemsBlended(m_renderItemsBlended);
    RenderDataManager::SubmitRenderItemsGlass(m_renderItemsGlass);
    RenderDataManager::SubmitRenderItemsAlphaHairTopLayer(m_renderItemsHairTopLayer);
    RenderDataManager::SubmitRenderItemsAlphaHairBottomLayer(m_renderItemsHairBottomLayer);
    RenderDataManager::SubmitRenderItemsMirror(m_renderItemsMirror);
}

const void MeshNodes::SubmitOutlineRenderItems() const {
    RenderDataManager::SubmitOutlineRenderItems(m_renderItems);
    RenderDataManager::SubmitOutlineRenderItems(m_renderItemsAlphaDiscarded);
    RenderDataManager::SubmitOutlineRenderItems(m_renderItemsBlended);
    RenderDataManager::SubmitOutlineRenderItems(m_renderItemsGlass);
    RenderDataManager::SubmitOutlineRenderItems(m_renderItemsHairTopLayer);
    RenderDataManager::SubmitOutlineRenderItems(m_renderItemsHairBottomLayer);
    RenderDataManager::SubmitOutlineRenderItems(m_renderItemsMirror);
}

const glm::mat4& MeshNodes::GetLocalModelMatrix(int nodeIndex) const {
    static const glm::mat4 identity = glm::mat4(1.0f);
    if (nodeIndex < 0) return identity;

    size_t i = static_cast<size_t>(nodeIndex);
    if (i >= m_meshNodes.size()) return identity;

    return m_meshNodes[i].localModelMatrix;
}

const glm::mat4& MeshNodes::GetWorldModelMatrix(int nodeIndex) const {
    static const glm::mat4 identity = glm::mat4(1.0f);
    if (nodeIndex < 0) return identity;

    size_t i = static_cast<size_t>(nodeIndex);
    if (i >= m_meshNodes.size()) return identity;

    return m_meshNodes[i].worldModelMatrix;
}

const glm::mat4& MeshNodes::GetBoneLocalMatrix(const std::string& boneName) const {
    static glm::mat4 identity = glm::mat4(1.0f);

    for (const Bone& bone : m_armatureData.bones) {
        if (bone.name == boneName) {
            return bone.localRestPose;
        }
    }
    return identity;
}

const glm::mat4& MeshNodes::GetLocalTransform(int nodeIndex) const {
    static const glm::mat4 identity = glm::mat4(1.0f);
    if (nodeIndex < 0) return identity;

    size_t i = static_cast<size_t>(nodeIndex);
    if (i >= m_meshNodes.size()) return identity;

    return m_meshNodes[i].localTransform;
}

const glm::mat4& MeshNodes::GetInverseBindTransform(int nodeIndex) const {
    static const glm::mat4 identity = glm::mat4(1.0f);
    if (nodeIndex < 0) return identity;

    size_t i = static_cast<size_t>(nodeIndex);
    if (i >= m_meshNodes.size()) return identity;

    return m_meshNodes[i].inverseBindTransform; 
}

const std::string& MeshNodes::GetMeshNameByNodeIndex(int nodeIndex) const {
    static std::string notFound = "NOT_FOUND";

    for (const auto& [name, idx] : m_localIndexMap) {
        if (idx == nodeIndex) return name;
    }
    return notFound;
}

MeshNode* MeshNodes::GetMeshNodeByLocalIndex(int32_t nodeIndex) {
    if (nodeIndex < 0 || nodeIndex >= (int32_t)m_meshNodes.size()) return nullptr;
    return &m_meshNodes[nodeIndex];
}

MeshNode* MeshNodes::GetMeshNodeByMeshName(const std::string& meshName) {
    int32_t nodeIndex = GetMeshNodeIndexByMeshName(meshName);
    if (nodeIndex == -1 || nodeIndex >= (int32_t)m_meshNodes.size()) return nullptr;
    return &m_meshNodes[(size_t)nodeIndex];
}

int32_t MeshNodes::GetMeshNodeIndexByMeshName(const std::string& meshName) {
    auto it = m_localIndexMap.find(meshName);
    if (it == m_localIndexMap.end()) return -1;
    return (int32_t)it->second;
}