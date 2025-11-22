#include "World.h"
#include "AssetManagement/AssetManager.h"
#include "Core/Game.h"
#include "Editor/Editor.h"
#include "HellLogging.h"
#include "Input/Input.h"
#include "Renderer/RenderDataManager.h"
#include "Renderer/Renderer.h"

namespace World {

    std::vector<RenderItem> g_skinnedRenderItems;

    void SubmitRenderItems() {
        for (GameObject& gameObject : GetGameObjects()) {
            gameObject.UpdateRenderItems();
            RenderDataManager::SubmitRenderItems(gameObject.GetRenderItems());
            RenderDataManager::SubmitRenderItemsBlended(gameObject.GetRenderItemsBlended());
            RenderDataManager::SubmitRenderItemsAlphaDiscard(gameObject.GetRenderItemsAlphaDiscarded());
            RenderDataManager::SubmitRenderItemsAlphaHairTopLayer(gameObject.GetRenderItemsHairTopLayer());
            RenderDataManager::SubmitRenderItemsAlphaHairBottomLayer(gameObject.GetRenderItemsHairBottomLayer());

            // Selected outline?
            if (gameObject.IsSelected()) {
                RenderDataManager::SubmitOutlineRenderItems(gameObject.GetRenderItems());
                RenderDataManager::SubmitOutlineRenderItems(gameObject.GetRenderItemsHairTopLayer());
                RenderDataManager::SubmitOutlineRenderItems(gameObject.GetRenderItemsHairBottomLayer());
            }
        }

        // Clear global render item vectors
        g_skinnedRenderItems.clear();

        for (PickUp& pickUp : GetPickUps()) {
            RenderDataManager::SubmitRenderItems(pickUp.GetRenderItems());
        }

        for (PictureFrame& pictureFrame : GetPictureFrames()) {
            RenderDataManager::SubmitRenderItems(pictureFrame.GetRenderItems());
            if (Editor::GetSelectedObjectId() == pictureFrame.GetObjectId()) {
                RenderDataManager::SubmitOutlineRenderItems(pictureFrame.GetRenderItems());
            }
        }

        for (Mermaid& mermaid: GetMermaids()) {
            RenderDataManager::SubmitRenderItems(mermaid.GetRenderItems());
            RenderDataManager::SubmitRenderItemsBlended(mermaid.GetRenderItemsBlended());
            RenderDataManager::SubmitRenderItemsAlphaDiscard(mermaid.GetRenderItemsAlphaDiscarded());
            RenderDataManager::SubmitRenderItemsAlphaHairTopLayer(mermaid.GetRenderItemsHairTopLayer());
            RenderDataManager::SubmitRenderItemsAlphaHairBottomLayer(mermaid.GetRenderItemsHairBottomLayer());
        }

        for (Decal& decal : GetDecals()) {
            decal.SubmitRenderItem();
        }

        for (HousePlane& housePlane : GetHousePlanes()) {
            housePlane.SubmitRenderItem();
        }

        // Doors
        for (Door& door : GetDoors()) {
            door.SubmitRenderItems();
        }
        
        for (Piano& piano : GetPianos()) {
            RenderDataManager::SubmitRenderItems(piano.GetRenderItems());
            if (Editor::GetSelectedObjectId() == piano.GetObjectId()) {
                RenderDataManager::SubmitOutlineRenderItems(piano.GetRenderItems());
            }
        }

        // Generic Object 
        for (GenericObject& genericGameObject : GetGenericObjects()) {
            if (Editor::GetSelectedObjectId() == genericGameObject.GetObjectId()) {
                genericGameObject.GetMeshNodes().SubmitOutlineRenderItems();
            }
            genericGameObject.GetMeshNodes().SubmitRenderItems();
        }

        // Window
        for (Window& window : GetWindows()) {
            if (Editor::GetSelectedObjectId() == window.GetObjectId()) {
                window.GetMeshNodes().SubmitOutlineRenderItems();
            }
            window.GetMeshNodes().SubmitRenderItems();
        }

        // Trees
        for (Tree& tree : GetTrees()) {
            RenderDataManager::SubmitRenderItems(tree.GetRenderItems());
            RenderDataManager::SubmitRenderItemsBlended(tree.GetRenderItemsBlended());
            RenderDataManager::SubmitRenderItemsAlphaDiscard(tree.GetRenderItemsAlphaDiscarded());
            RenderDataManager::SubmitRenderItemsAlphaHairTopLayer(tree.GetRenderItemsHairTopLayer());
            RenderDataManager::SubmitRenderItemsAlphaHairBottomLayer(tree.GetRenderItemsHairBottomLayer());
            if (Editor::GetSelectedObjectId() == tree.GetObjectId()) {
                RenderDataManager::SubmitOutlineRenderItems(tree.GetRenderItems());
                RenderDataManager::SubmitOutlineRenderItems(tree.GetRenderItemsAlphaDiscarded());
                RenderDataManager::SubmitOutlineRenderItems(tree.GetRenderItemsBlended());
            }
        }

        // Lights
        for (Light& light : GetLights()) {
            RenderDataManager::SubmitRenderItems(light.GetRenderItems());
        }

        for (BulletCasing& bulletCasing : GetBulletCasings()) {
            bulletCasing.SubmitRenderItem();
        }

        RenderDataManager::ResetBaseSkinnedVertex();
        for (AnimatedGameObject& animatedGameObject : GetAnimatedGameObjects()) {
            if (animatedGameObject.RenderingEnabled()) {
                animatedGameObject.UpdateRenderItems();
                animatedGameObject.SubmitForSkinning();
                g_skinnedRenderItems.insert(g_skinnedRenderItems.end(), animatedGameObject.GetRenderItems().begin(), animatedGameObject.GetRenderItems().end());
            }
        }

        for (TrimSet& trimSet : GetTrimSets()) {
            RenderDataManager::SubmitRenderItems(trimSet.GetRenderItems());
        }

        for (Fireplace& fireplace: GetFireplaces()) {
            RenderDataManager::SubmitRenderItems(fireplace.GetRenderItems());             
            if (Editor::GetSelectedObjectId() == fireplace.GetObjectId()) {
                RenderDataManager::SubmitOutlineRenderItems(fireplace.GetRenderItems());
            }
        }

        for (Wall& wall : GetWalls()) {
            wall.SubmitRenderItems();
            RenderDataManager::SubmitRenderItems(wall.GetWeatherBoardstopRenderItems());
        }

        for (PowerPoleSet& powerPoleSet : GetPowerPoleSets()) {
            RenderDataManager::SubmitRenderItems(powerPoleSet.GetRenderItems());
        }

        for (Fence& fence: GetFences()) {
            RenderDataManager::SubmitRenderItems(fence.GetRenderItems());
        }

        for (ChristmasLights& christmasLights : GetChristmasLights()) {
            RenderDataManager::SubmitRenderItems(christmasLights.GetRenderItems());
        }

        for (ChristmasPresent& christmasPresent : GetChristmasPresents()) {
            RenderDataManager::SubmitRenderItems(christmasPresent.GetRenderItems());
        }

        for (ChristmasTree& christmasTree : GetChristmasTrees()) {
            RenderDataManager::SubmitRenderItemsAlphaDiscard(christmasTree.GetRenderItems());
        }

        // Hack to render door and window cube transforms
        if (true && false) {
            int meshIndex = AssetManager::GetMeshIndexByModelNameMeshName("Primitives", "Cube");
            std::vector<Transform>& transforms = GetDoorAndWindowCubeTransforms();
            for (Transform& transform : transforms) {
                Material* material = AssetManager::GetDefaultMaterial();
                RenderItem renderItem;
                renderItem.meshIndex = meshIndex;
                renderItem.modelMatrix = transform.to_mat4();
                renderItem.baseColorTextureIndex = material->m_basecolor;
                renderItem.normalMapTextureIndex = material->m_normal;
                renderItem.rmaTextureIndex = material->m_rma;
                Util::UpdateRenderItemAABB(renderItem);
                RenderDataManager::SubmitRenderItem(renderItem);
            }
        }

        // Animated game objects
        for (int i = 0; i < Game::GetLocalPlayerCount(); i++) {
            Player* player = Game::GetLocalPlayerByIndex(i);
            AnimatedGameObject* viewWeapon = player->GetViewWeaponAnimatedGameObject();
            AnimatedGameObject* characterModel = player->GetCharacterModelAnimatedGameObject();

            if (player->ShouldRenderViewWeapon()) {
                viewWeapon->UpdateRenderItems();
                viewWeapon->SubmitForSkinning();
            }

            characterModel->UpdateRenderItems();
            characterModel->SubmitForSkinning();
            g_skinnedRenderItems.insert(g_skinnedRenderItems.end(), viewWeapon->GetRenderItems().begin(), viewWeapon->GetRenderItems().end());
            g_skinnedRenderItems.insert(g_skinnedRenderItems.end(), characterModel->GetRenderItems().begin(), characterModel->GetRenderItems().end());
        }

        // Player render items
        for (int i = 0; i < Game::GetLocalPlayerCount(); i++) {
            Player* player = Game::GetLocalPlayerByIndex(i);
            player->SubmitRenderItems();
        }
    }

    std::vector<RenderItem>& GetSkinnedRenderItems() { return g_skinnedRenderItems; }
}