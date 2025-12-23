#include "World.h"
#include "AssetManagement/AssetManager.h"
#include "Bvh/Cpu/CpuBvh.h";
#include "Editor/Editor.h"
#include "Input/Input.h"
#include "Renderer/Renderer.h"
#include "Timer.hpp"

namespace World {
	std::vector<PrimitiveInstance> g_dynamicSceneInstances;
	std::vector<PrimitiveInstance> g_staticSceneInstances;
	uint64_t g_dynamicSceneBvhId = 0;
	uint64_t g_staticSceneBvhId = 0;
	bool g_staticBvhSceneDirty = true;

	void DebugDraw();
	void UpdateDynamicBvhScene();
	void UpdateStaticBvhScene();


	void UpdateBvhs() {
		if (g_staticBvhSceneDirty) {
			g_staticBvhSceneDirty = false;
			UpdateStaticBvhScene();
		}

		UpdateDynamicBvhScene();
		//DebugDraw();
	}

    void CreateObjectInstanceDataFromRenderItem(const RenderItem& renderItem, std::vector<PrimitiveInstance>& container) {
        PrimitiveInstance& instance = container.emplace_back();
		instance.worldTransform = renderItem.modelMatrix;
		instance.inverseWorldTransform = renderItem.inverseModelMatrix;
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


    void CreateObjectInstanceDataFromRenderItems(const std::vector<RenderItem>& renderItems, std::vector<PrimitiveInstance>& container) {
        for (const RenderItem& renderItem : renderItems) {
            CreateObjectInstanceDataFromRenderItem(renderItem, container);
        }
    }

	void CreatePrimtiveInstanceFromMeshNode(const MeshNode* meshNode, std::vector<PrimitiveInstance>& container) {
        if (!meshNode) return;

		PrimitiveInstance& instance = container.emplace_back();
		instance.worldTransform = meshNode->worldMatrix;
		instance.inverseWorldTransform = meshNode->inverseWorldMatrix;
		instance.worldAabbBoundsMin = meshNode->worldspaceAabb.GetBoundsMin();
		instance.worldAabbBoundsMax = meshNode->worldspaceAabb.GetBoundsMax();
		instance.worldAabbCenter = meshNode->worldspaceAabb.GetCenter();
		instance.meshBvhId = meshNode->meshBvhId;
		instance.openableId = meshNode->openableId;
		instance.globalMeshIndex = meshNode->globalMeshIndex;
		instance.customId = meshNode->customId;
		instance.localMeshNodeIndex = meshNode->nodeIndex;
        instance.objectId = meshNode->parentObjectId;
	}


	void DebugDraw() {
		for (PrimitiveInstance& primitiveInstance : g_dynamicSceneInstances) {
			Renderer::DrawAABB(AABB(primitiveInstance.worldAabbBoundsMin, primitiveInstance.worldAabbBoundsMax), YELLOW);
		}
		for (PrimitiveInstance& primitiveInstance : g_staticSceneInstances) {
			Renderer::DrawAABB(AABB(primitiveInstance.worldAabbBoundsMin, primitiveInstance.worldAabbBoundsMax), GREEN);
		}
	}


	void CreateDynamicPrimtiveInstances(MeshNodes& meshNodes) {
		for (int i = 0; i < meshNodes.GetNodeCount(); i++) {
			MeshNode* meshNode = meshNodes.GetMeshNodeByLocalIndex(i);
			if (!meshNodes.MeshNodeIsStatic(i)) {
				CreatePrimtiveInstanceFromMeshNode(meshNode, g_dynamicSceneInstances);
			}
		}
	}    
    

    void CreateStaticPrimtiveInstances(MeshNodes& meshNodes) {
		for (int i = 0; i < meshNodes.GetNodeCount(); i++) {
			MeshNode* meshNode = meshNodes.GetMeshNodeByLocalIndex(i);
			if (meshNodes.MeshNodeIsStatic(i)) {
				CreatePrimtiveInstanceFromMeshNode(meshNode, g_staticSceneInstances);
			}
		}
	}


	void UpdateDynamicBvhScene() {
		//Timer timer("DynamicBvhSceneUpdate");

		// Create scene if it doesn't exist
		if (g_dynamicSceneBvhId == 0) {
            g_dynamicSceneBvhId = Bvh::Cpu::CreateNewSceneBvh();
		}

		// Clear any existing primitive instances
		g_dynamicSceneInstances.clear();

		// Add any dynamic mesh nodes to the primitive instances vector
		for (Door& door : GetDoors()) {
			CreateDynamicPrimtiveInstances(door.GetMeshNodes());
		}
		for (Fireplace& fireplace : GetFireplaces()) {
            CreateDynamicPrimtiveInstances(fireplace.GetMeshNodes());
		}
		for (GenericObject& genericObject : GetGenericObjects()) {
			CreateDynamicPrimtiveInstances(genericObject.GetMeshNodes());
        }
        for (Piano& piano : GetPianos()) {
            CreateDynamicPrimtiveInstances(piano.GetMeshNodes());
        }

        // TODO: Remove me
		//for (Door& door : GetDoors()) {
		//	const std::vector<RenderItem>& renderItems = door.GetRenderItems();
		//	for (const RenderItem& renderItem : renderItems) {
		//		CreateObjectInstanceDataFromRenderItem(renderItem, g_dynamicSceneInstances);
		//	}
		//}
		for (PictureFrame& pictureFrame : GetPictureFrames()) {
			const std::vector<RenderItem>& renderItems = pictureFrame.GetRenderItems();
			for (const RenderItem& renderItem : renderItems) {
				CreateObjectInstanceDataFromRenderItem(renderItem, g_dynamicSceneInstances);
			}
		}
		for (PickUp& pickUp : GetPickUps()) {
			const std::vector<RenderItem>& renderItems = pickUp.GetRenderItems();
			for (const RenderItem& renderItem : renderItems) {
				CreateObjectInstanceDataFromRenderItem(renderItem, g_dynamicSceneInstances);
			}
		}
		if (Editor::IsOpen()) {
			for (Tree& tree : GetTrees()) {
				for (const RenderItem& renderItem : tree.GetRenderItems()) {
					CreateObjectInstanceDataFromRenderItem(renderItem, g_dynamicSceneInstances);
				}
				for (const RenderItem& renderItem : tree.GetRenderItemsAlphaDiscarded()) {
					CreateObjectInstanceDataFromRenderItem(renderItem, g_dynamicSceneInstances);
				}
			}
		}





		// Recreate the TLAS
        Bvh::Cpu::UpdateSceneBvh(g_dynamicSceneBvhId, g_dynamicSceneInstances);
	}


	void UpdateStaticBvhScene() {
        // Create scene if it doesn't exist
		if (g_staticSceneBvhId == 0) {
			g_staticSceneBvhId = Bvh::Cpu::CreateNewSceneBvh();
		}

        // Clear any existing primitive instances
		g_staticSceneInstances.clear();

        // Render items
        for (ChristmasLights& christmasLights : GetChristmasLights())	CreateObjectInstanceDataFromRenderItems(christmasLights.GetRenderItems(), g_staticSceneInstances);

        // Add any static mesh nodes to the primitive instances vector
        for (Door& object : GetDoors())                     CreateStaticPrimtiveInstances(object.GetMeshNodes());
        for (Fireplace& object : GetFireplaces())           CreateStaticPrimtiveInstances(object.GetMeshNodes());
        for (GenericObject& object : GetGenericObjects())   CreateStaticPrimtiveInstances(object.GetMeshNodes());
        for (Ladder& object : GetLadders())                 CreateStaticPrimtiveInstances(object.GetMeshNodes());
        for (Light& object : GetLights())					CreateStaticPrimtiveInstances(object.GetMeshNodes());
        for (Piano& object : GetPianos())                   CreateStaticPrimtiveInstances(object.GetMeshNodes());
        for (Window& object : GetWindows())                 CreateStaticPrimtiveInstances(object.GetMeshNodes());

        // Recreate the TLAS
		Bvh::Cpu::UpdateSceneBvh(g_staticSceneBvhId, g_staticSceneInstances);
		std::cout << "Updated static scene Bvh\n";
    }


	BvhRayResult ClosestHit(glm::vec3 rayOrigin, glm::vec3 rayDir, float maxRayDistance) {
		// Bail if invalid ray direction
        if (Util::IsNan(rayDir)) {
			return BvhRayResult();
		}

		// First check for a hit with the static scene
		BvhRayResult staticResult = Bvh::Cpu::ClosestHit(g_staticSceneBvhId, rayOrigin, rayDir, maxRayDistance);

        // If a hit was found, then update the max ray distance so you don't search further than you need to in the dynamic scene raycast
        if (staticResult.hitFound) {
            maxRayDistance = staticResult.distanceToHit;
        }

        BvhRayResult dynamicResult = Bvh::Cpu::ClosestHit(g_dynamicSceneBvhId, rayOrigin, rayDir, maxRayDistance);

        // Dynamic scene hit was closest
        if (dynamicResult.hitFound) {
            return dynamicResult;
        }
        // Otherwise return the static result, which may or may not be a hit
        else {
            return staticResult;
        }
	}


	void MarkStaticSceneBvhDirty() {
		g_staticBvhSceneDirty = true;
	}
}