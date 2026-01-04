#include "Player.h"
#include "AssetManagement/AssetManager.h"
#include "Bible/Bible.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderDataManager.h"
#include "Viewport/ViewportManager.h"
#include "Util.h"

void Player::SubmitAttachmentRenderItem(const std::string& weaponAttachmentName) {
    WeaponAttachmentInfo* weaponAttachmentInfo = Bible::GetWeaponAttachmentInfoByName(weaponAttachmentName);
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();

    if (!weaponInfo) {
        std::cout << "Player::SubmitAttachmentRenderItem() failed because weaponInfo was nullptr\n";
        return;
    }

    if (!weaponAttachmentInfo) {
        std::cout << "Player::SubmitAttachmentRenderItem() failed because weaponAttachmentName '" << weaponAttachmentName << "' was not found\n";
        return;
    }

    if (!viewWeapon) {
        std::cout << "Player::SubmitAttachmentRenderItem() failed because viewWeapon was nullptr\n";
        return;
    }

    Model* model = AssetManager::GetModelByName(weaponAttachmentInfo->modelName);
    if (!model) {
        std::cout << "Player::SubmitAttachmentRenderItem() failed because weaponAttachmentInfo->modelName '" << weaponAttachmentInfo->modelName << "' was not found\n";
        return;
    }

    for (int i = 0; i < model->GetMeshCount(); i++) {
        int meshIndex = model->GetMeshIndices()[i];
        Mesh* mesh = AssetManager::GetMeshByIndex(meshIndex);
        if (!mesh) continue;

        Material* material = AssetManager::GetDefaultMaterial();
        
        // Get material (Clean this up later for the love of GAWD!!!!!!!!!!)
        for (const auto& pair : weaponAttachmentInfo->meshMaterialNames) {
            const std::string& meshName = pair.first;
            const std::string& materialName = pair.second;
            if (mesh->GetName() == meshName) {
                material = AssetManager::GetMaterialByName(materialName);
            }
        }


        // Is this mesh glass? ( Clean this up later for the love of GAWD!!!!!!!!!!)
        bool meshIsGlass = false;
        for (std::string& glassMeshName : weaponAttachmentInfo->glassMeshNames) {
            if (mesh->GetName() == glassMeshName) {
                meshIsGlass = true;
                break;
            }
        }

        RenderItem renderItem;
        renderItem.modelMatrix = viewWeapon->GetBoneWorldMatrix(weaponAttachmentInfo->boneName);
        renderItem.inverseModelMatrix = inverse(renderItem.modelMatrix);
        renderItem.baseColorTextureIndex = material->m_basecolor;
        renderItem.rmaTextureIndex = material->m_rma;
        renderItem.normalMapTextureIndex = material->m_normal;
        renderItem.meshIndex = meshIndex;
        renderItem.exclusiveViewportIndex = m_viewportIndex;
        renderItem.castShadows = false;
        Util::UpdateRenderItemAABB(renderItem);

        // Emissive texture
        for (const auto& pair : weaponAttachmentInfo->meshEmmisveTextureNames) {
            const std::string& meshName = pair.first;
            const std::string& textureName = pair.second;
            if (mesh->GetName() == meshName) {
                renderItem.emissiveTextureIndex = AssetManager::GetTextureIndexByName(textureName);
            }
        }

        if (meshIsGlass) {
            RenderDataManager::SubmitGlassRenderItem(renderItem);
        }
        else {
            RenderDataManager::SubmitRenderItem(renderItem);
        }
    }
}

void Player::SubmitRenderItems() {
    WeaponInfo* weaponInfo = GetCurrentWeaponInfo();
    WeaponState* weaponState = GetWeaponStateByName(weaponInfo->itemInfoName);
    AnimatedGameObject* viewWeapon = GetViewWeaponAnimatedGameObject();
    Viewport* viewport = ViewportManager::GetViewportByIndex(m_viewportIndex);

    if (!weaponState) return;
    if (!weaponInfo) return;
    if (!viewWeapon) return;
    if (!viewport) return;
    if (!viewport->IsVisible()) return;

    if (ShouldRenderViewWeapon() && weaponState->hasSilencer) {
        SubmitAttachmentRenderItem(weaponInfo->silencerName);
    }

    if (ShouldRenderViewWeapon() && weaponState->hasSight) {
        SubmitAttachmentRenderItem(weaponInfo->sightName);
    }       
}