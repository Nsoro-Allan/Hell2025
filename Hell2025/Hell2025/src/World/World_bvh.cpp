#include "World.h"
#include "AssetManagement/AssetManager.h"
#include "Bvh/Cpu/CpuBvh.h";
#include "Core/Game.h"
#include "Editor/Editor.h"
#include "Renderer/RenderDataManager.h"
#include "Viewport/ViewportManager.h"
#include "Renderer/Renderer.h"
#include "Input/Input.h"

namespace World {
          
    ViewportBvhData g_viewportBvhData[4];

    void CreateObjectInstanceDataFromRenderItem(const RenderItem& renderItem, Frustum& frustum, std::vector<PrimitiveInstance>& container) {
        if (frustum.IntersectsAABBFast(renderItem)) {
            PrimitiveInstance& instance = container.emplace_back();
            instance.worldTransform = renderItem.modelMatrix;
            instance.worldAabbBoundsMin = renderItem.aabbMin;
            instance.worldAabbBoundsMax = renderItem.aabbMax;
            instance.worldAabbCenter = (renderItem.aabbMin + renderItem.aabbMax) * 0.5f;
            instance.meshBvhId = AssetManager::GetMeshByIndex(renderItem.meshIndex)->meshBvhId;
            instance.openableId= renderItem.openableId;
            instance.globalMeshIndex = renderItem.meshIndex;
            instance.customId = renderItem.customId;
            instance.localMeshNodeIndex = renderItem.localMeshNodeIndex;
            Util::UnpackUint64(renderItem.objectIdLowerBit, renderItem.objectIdUpperBit, instance.objectId);
        }
    }

    void UpdatePlayerBvhs() {

        for (int i = 0; i < 4; i++) {
            Viewport* viewport = ViewportManager::GetViewportByIndex(i);
            if (!viewport->IsVisible()) continue;

            ViewportBvhData& viewportBvhData = g_viewportBvhData[i];
            viewportBvhData.instances.clear();

            Player* player = Game::GetLocalPlayerByIndex(i);
            if (!player) continue;

            Frustum frustum = player->GetFlashlightFrustum();

            // If the editor is open, reset the frustum to the editor viewport
            if (Editor::IsOpen()) {
                frustum.Update(RenderDataManager::GetViewportData()[i].projectionView);
            }
            
            for (Door& door : GetDoors()) {
                const std::vector<RenderItem>& renderItems = door.GetRenderItems();
                for (const RenderItem& renderItem : renderItems) {
                    CreateObjectInstanceDataFromRenderItem(renderItem, frustum, viewportBvhData.instances);
                }
            }

            for (GameObject& gameObject : GetGameObjects()) {
                const std::vector<RenderItem>& renderItems = gameObject.GetRenderItems();
                for (const RenderItem& renderItem : renderItems) {
                    CreateObjectInstanceDataFromRenderItem(renderItem, frustum, viewportBvhData.instances);
                }
            }

            for (Light& light : GetLights()) {
                const std::vector<RenderItem>& renderItems = light.GetRenderItems();
                for (const RenderItem& renderItem : renderItems) {
                    CreateObjectInstanceDataFromRenderItem(renderItem, frustum, viewportBvhData.instances);
                }
            }

            for (Piano& piano : GetPianos()) {
                const std::vector<RenderItem>& renderItems = piano.GetRenderItems();
                for (const RenderItem& renderItem : renderItems) {
                    CreateObjectInstanceDataFromRenderItem(renderItem, frustum, viewportBvhData.instances);
                }
            }

            for (PictureFrame& pictureFrame : GetPictureFrames()) {
                const std::vector<RenderItem>& renderItems = pictureFrame.GetRenderItems();
                for (const RenderItem& renderItem : renderItems) {
                    CreateObjectInstanceDataFromRenderItem(renderItem, frustum, viewportBvhData.instances);
                }
            }

            for (PickUp& pickUp : GetPickUps()) {
                const std::vector<RenderItem>& renderItems = pickUp.GetRenderItems();
                for (const RenderItem& renderItem : renderItems) {
                    CreateObjectInstanceDataFromRenderItem(renderItem, frustum, viewportBvhData.instances);
                }
            }

            for (Piano& piano : GetPianos()) {
                const std::vector<RenderItem>& renderItems = piano.GetRenderItems();
                for (const RenderItem& renderItem : renderItems) {
                    CreateObjectInstanceDataFromRenderItem(renderItem, frustum, viewportBvhData.instances);
                }
            }

            for (GenericObject& genericObject : GetGenericObjects()) {
                for (const RenderItem& renderItem : genericObject.GetRenderItems()) {
                    CreateObjectInstanceDataFromRenderItem(renderItem, frustum, viewportBvhData.instances);
                }
                for (const RenderItem& renderItem : genericObject.GetRenderItemsHairTopLayer()) {
                    CreateObjectInstanceDataFromRenderItem(renderItem, frustum, viewportBvhData.instances);
                }
            }

            for (Fireplace& fireplace : GetFireplaces()) {
                const std::vector<RenderItem>& renderItems = fireplace.GetRenderItems();
                for (const RenderItem& renderItem : renderItems) {
                    CreateObjectInstanceDataFromRenderItem(renderItem, frustum, viewportBvhData.instances);
                }
            }

            if (Editor::IsOpen()) {
                for (Tree& tree : GetTrees()) {
                    for (const RenderItem& renderItem : tree.GetRenderItems()) {
                        CreateObjectInstanceDataFromRenderItem(renderItem, frustum, viewportBvhData.instances);
                    }
                    for (const RenderItem& renderItem : tree.GetRenderItemsAlphaDiscarded()) {
                        CreateObjectInstanceDataFromRenderItem(renderItem, frustum, viewportBvhData.instances);
                    }
                }
            }
            
            for (Window& window : GetWindows()) {
                const std::vector<RenderItem>& renderItems = window.GetRenderItems();
                for (const RenderItem& renderItem : renderItems) {
                    CreateObjectInstanceDataFromRenderItem(renderItem, frustum, viewportBvhData.instances);
                }
            }

            // Rebuild TLAS
            if (viewportBvhData.sceneBvhId == 0) {
                viewportBvhData.sceneBvhId = Bvh::Cpu::CreateNewSceneBvh();
            }
            Bvh::Cpu::UpdateSceneBvh(viewportBvhData.sceneBvhId, viewportBvhData.instances);
        }
    }

    uint64_t GetSceneBvhIdByViewportIndex(int viewportIndex) {
        if (viewportIndex < 0 || viewportIndex >= 4) return 0;
        return g_viewportBvhData[viewportIndex].sceneBvhId;
    }

    BvhRayResult ClosestHit(glm::vec3 rayOrigin, glm::vec3 rayDir, float maxRayDistance, int viewportIndex) {
        std::vector<PrimitiveInstance>& instances = g_viewportBvhData[viewportIndex].instances;
        uint64_t& sceneBvhId = g_viewportBvhData[viewportIndex].sceneBvhId;
        if (Util::IsNan(rayDir)) {
            return BvhRayResult();
        }
        return Bvh::Cpu::ClosestHit(sceneBvhId, rayOrigin, rayDir, maxRayDistance);;
    }

    //void RemoveAllHouseBvhs() {
    //    
    //}
    //
    //void AddHouseBvh() {
    //
    //}
}