#include "World.h"
#include "AssetManagement/AssetManager.h"
#include "Core/Game.h"
#include "Editor/Editor.h"
#include "Input/Input.h"
#include "Renderer/RenderDataManager.h"
#include "Renderer/Renderer.h"

namespace World {

    std::vector<RenderItem> g_skinnedRenderItems;

    void SubmitRenderItems() {
        std::vector<AnimatedGameObject>& animatedGameObjects = GetAnimatedGameObjects();
        std::vector<BulletCasing>& bulletCasings = GetBulletCasings();
        std::vector<Decal>& decals = GetDecals();
        std::vector<Door>& doors = GetDoors();
        std::vector<GameObject>& gameObjects = GetGameObjects();
        std::vector<Light>& lights = GetLights();
        std::vector<PickUp>& pickUps = GetPickUps();
        std::vector<Piano>& pianos = GetPianos();
        std::vector<Tree>& trees = GetTrees();
        std::vector<Wall>& walls = GetWalls();
        std::vector<Window>& windows = GetWindows();

        for (GameObject& gameObject : gameObjects) {
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

        for (PickUp& pickUp : pickUps) {
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

        for (Decal& decal : decals) {
            decal.SubmitRenderItem();
        }

        for (Plane& housePlane : GetPlanes()) {
            housePlane.SubmitRenderItem();
        }

        // Doors
        for (Door& door : doors) {
            door.SubmitRenderItems();
        }

        for (Piano& piano : pianos) {
            RenderDataManager::SubmitRenderItems(piano.GetRenderItems());
            if (Editor::GetSelectedObjectId() == piano.GetObjectId()) {
                RenderDataManager::SubmitOutlineRenderItems(piano.GetRenderItems());
            }
        }

        // Toilets
        for (Toilet& toilet : GetToilets()) {
            RenderDataManager::SubmitRenderItems(toilet.GetRenderItems());
            // TODO: if (Editor::GetSelectedObjectId() == toilet.GetObjectId()) {
            // TODO:     RenderDataManager::SubmitOutlineRenderItems(toilet.GetRenderItems());
            // TODO: }
        }

        // Drawers
        for (Drawers& drawers : GetDrawers()) {
            RenderDataManager::SubmitRenderItems(drawers.GetRenderItems());
            // TODO: if (Editor::GetSelectedObjectId() == toilet.GetObjectId()) {
            // TODO:     RenderDataManager::SubmitOutlineRenderItems(toilet.GetRenderItems());
            // TODO: }
        }

        // Window
        for (Window& window : windows) {
            window.SubmitRenderItems();
        }

        // Trees
        for (Tree& tree : trees) {
            RenderDataManager::SubmitRenderItems(tree.GetRenderItems());
            RenderDataManager::SubmitRenderItemsBlended(tree.GetRenderItemsBlended());
            RenderDataManager::SubmitRenderItemsAlphaDiscard(tree.GetRenderItemsAlphaDiscarded());
            RenderDataManager::SubmitRenderItemsAlphaHairTopLayer(tree.GetRenderItemsHairTopLayer());
            RenderDataManager::SubmitRenderItemsAlphaHairBottomLayer(tree.GetRenderItemsHairBottomLayer());
            if (Editor::GetSelectedObjectId() == tree.GetObjectId()) {
                RenderDataManager::SubmitOutlineRenderItems(tree.GetRenderItems());
            }
        }

        // Lights
        for (Light& light : lights) {
            RenderDataManager::SubmitRenderItems(light.GetRenderItems());
        }

        for (BulletCasing& bulletCasing : bulletCasings) {
            bulletCasing.SubmitRenderItem();
        }

        RenderDataManager::ResetBaseSkinnedVertex();
        for (AnimatedGameObject& animatedGameObject : animatedGameObjects) {
            animatedGameObject.UpdateRenderItems();
            animatedGameObject.SubmitForSkinning();
            g_skinnedRenderItems.insert(g_skinnedRenderItems.end(), animatedGameObject.GetRenderItems().begin(), animatedGameObject.GetRenderItems().end());
        }

        for (Wall& wall : GetWalls()) {
            wall.SubmitRenderItems();
            RenderDataManager::SubmitRenderItems(wall.GetWeatherBoardstopRenderItems());
        }

        for (GenericBouncable& genericBouncable : GetGenericBouncables()) {
            RenderDataManager::SubmitRenderItems(genericBouncable.GetRenderItems());
        }

        for (GenericStatic& genericStatics : GetGenericStatics()) {
            RenderDataManager::SubmitRenderItems(genericStatics.GetRenderItems());
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