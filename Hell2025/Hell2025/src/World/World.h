#pragma once
#include "HellTypes.h"
#include "SlotMap.h"

#include "Core/Debug.h"
#include "Types/Characters/Allies/Mermaid/Mermaid.h"
#include "Types/Characters/Enemies/Dobermann/Dobermann.h"
#include "Types/Characters/Enemies/Kangaroo/Kangaroo.h"
#include "Types/Characters/Enemies/Shark/Shark.h"
#include "Types/Christmas/ChristmasLights.h"
#include "Types/Christmas/ChristmasPresent.h"
#include "Types/Christmas/ChristmasTree.h"
#include "Types/Core/GenericObject.h"
#include "Types/Effects/VolumetricBloodSplatter.h"
#include "Types/Effects/ScreenSpaceBloodDecal.h"
#include "Types/Exterior/Fence.h"
#include "Types/Exterior/Road.h"
#include "Types/Exterior/PowerPoleSet.h"
#include "Types/Exterior/Tree.h"
#include "Types/Game/AnimatedGameObject.h"
#include "Types/Game/Bullet.h"
#include "Types/Game/BulletCasing.h"
#include "Types/Game/Decal.h"
#include "Types/Game/GameObject.h"
#include "Types/Game/Light.h"
#include "Types/Game/PickUp.h"
#include "Types/Generics/GenericBouncable.h"
#include "Types/Generics/GenericStatic.h"
#include "Types/Interior/PictureFrame.h"
#include "Types/Interior/Piano.h"
#include "Types/House/Door.h"
#include "Types/House/HouseInstance.h"
#include "Types/House/Plane.h"
#include "Types/House/Wall.h"
#include "Types/House/Window.h"
#include "Types/Map/Map.h"
#include "Types/Map/MapInstance.h"
#include "Util/Util.h"
#include "glm/gtx/intersect.hpp"
#include <vector>
#include "Modelling/Clipping.h"

#include "Types/Renderer/MeshBuffer.h"

struct ViewportBvhData {
    std::vector<PrimitiveInstance> instances;
    uint64_t sceneBvhId;
    BvhRayResult closestHit;
};

struct MapInstanceCreateInfo {
    std::string mapName;
    uint32_t spawnOffsetChunkX;
    uint32_t spawnOffsetChunkZ;
};

namespace World {
    void Init();
    void BeginFrame();
    void EndFrame();
    void Update(float deltaTime);

    void NewRun();

    void SubmitRenderItems();

    void RecreateHouseMesh();

    void ResetWorld();
    void ClearAllObjects();

    void LoadMapInstance(const std::string& mapName); // Calls the function below, but with a single map
    void LoadMapInstances(std::vector<MapInstanceCreateInfo> mapInstanceCreateInfoSet); // Calls the 3 functions below
    void LoadMapInstancesHeightMapData(std::vector<MapInstanceCreateInfo> mapInstanceCreateInfoSet);
    void LoadMapInstanceObjects(const std::string& mapName, SpawnOffset spawnOffset);
    void LoadMapInstanceHouses(const std::string& mapName, SpawnOffset spawnOffset);
    
    void LoadSingleHouse(const std::string& houseName);
    void LoadHouseInstance(const std::string& houseName, SpawnOffset spawnOffset);

    bool ChunkExists(int x, int z);
    const uint32_t GetChunkCountX();
    const uint32_t GetChunkCountZ();
    const uint32_t GetChunkCount(); 
    const HeightMapChunk* GetChunk(int x, int z);

    void AddBullet(BulletCreateInfo createInfo);
    void AddDoor(DoorCreateInfo createInfo, SpawnOffset spawnOffset = SpawnOffset());
    void AddBulletCasing(BulletCasingCreateInfo createInfo, SpawnOffset spawnOffset = SpawnOffset());
    void AddChristmasLights(ChristmasLightsCreateInfo createInfo, SpawnOffset spawnOffset = SpawnOffset());
    void AddChristmasPresent(ChristmasPresentCreateInfo createInfo, SpawnOffset spawnOffset = SpawnOffset());
    void AddChristmasTree(ChristmasTreeCreateInfo createInfo, SpawnOffset spawnOffset = SpawnOffset());
    void AddCreateInfoCollection(CreateInfoCollection& createInfoCollection, SpawnOffset spawnOffset);
    void AddDecal(const DecalCreateInfo& createInfo);
    void AddDobermann(DobermannCreateInfo& createInfo);
    void AddGenericObject(GenericObjectCreateInfo createInfo, SpawnOffset spawnOffset);
    void AddGameObject(GameObjectCreateInfo createInfo, SpawnOffset spawnOffset = SpawnOffset());
    void AddGenericStatic(GenericStaticCreateInfo createInfo, SpawnOffset spawnOffset = SpawnOffset());
    void AddGenericBouncable(GenericBouncableCreateInfo createInfo, SpawnOffset spawnOffset = SpawnOffset());
    void AddHousePlane(PlaneCreateInfo createInfo, SpawnOffset spawnOffset);
    void AddKangaroo(const KangarooCreateInfo& createInfo);
    void AddLight(LightCreateInfo createInfo, SpawnOffset spawnOffset = SpawnOffset());
    void AddMermaid(MermaidCreateInfo createInfo, SpawnOffset spawnOffset = SpawnOffset());
    void AddScreenSpaceBloodDecal(ScreenSpaceBloodDecalCreateInfo createInfo);
    void AddPiano(PianoCreateInfo createInfo, SpawnOffset spawnOffset);
    void AddPickUp(PickUpCreateInfo createInfo, SpawnOffset spawnOffset = SpawnOffset());
    void AddPictureFrame(PictureFrameCreateInfo createInfo, SpawnOffset spawnOffset = SpawnOffset());
    void AddToilet(ToiletCreateInfo createInfo, SpawnOffset spawnOffset = SpawnOffset());
    void AddTree(TreeCreateInfo createInfo, SpawnOffset spawnOffset = SpawnOffset());
    uint64_t AddWall(WallCreateInfo createInfo, SpawnOffset spawnOffset = SpawnOffset());
    void AddVATBlood(glm::vec3 position, glm::vec3 front);
    void AddWindow(WindowCreateInfo createInfo, SpawnOffset spawnOffset);

    //void AddHouse(HouseCreateInfo houseCreateInfo, SpawnOffset spawnOffset);

    void PrintObjectCounts();

    void EnableOcean();
    void DisableOcean();
    bool HasOcean();
    
    // Logic
    void ProcessBullets();

    // Creation
    void CreateGameObject();
    uint64_t CreateAnimatedGameObject();

    // Objects
    void SetObjectPosition(uint64_t objectId, glm::vec3 position);
    void SetObjectRotation(uint64_t objectId, glm::vec3 rotation);
    bool RemoveObject(uint64_t objectId);
    glm::vec3 GetGizmoOffest(uint64_t objectId);
    
    // BVH
    //void RemoveAllHouseBvhs();
    void UpdatePlayerBvhs();
    BvhRayResult ClosestHit(glm::vec3 rayOrigin, glm::vec3 rayDir, float maxRayDistance, int viewportIndex);

    const float GetWorldSpaceWidth();
    const float GetWorldSpaceDepth();

    void UpdateDoorAndWindowCubeTransforms();
    void ResetWeatherboardMeshBuffer();
    void UpdateAllHangingLightCords();

    // Util
    bool ObjectTypeIsInteractable(ObjectType objectType, uint64_t objectId, glm::vec3 playerCameraPosition, glm::vec3 rayHitPosition);

    // Map
    const std::string& GetCurrentMapName();

    // House
    void SaveHouse();
    void UpdateClippingCubes();
    void UpdateAllWallCSG();
    void UpdateHouseMeshBuffer();
    void UpdateWeatherBoardMeshBuffer();

    // Spawns
    SpawnPoint GetRandomCampaignSpawnPoint();
    SpawnPoint GetRandomDeathmanSpawnPoint();
    void UpdateWorldSpawnPointsFromMap(Map* map);

    MeshBuffer& GetHouseMeshBuffer();
    MeshBuffer& GetWeatherBoardMeshBuffer();
    Mesh* GetHouseMeshByIndex(uint32_t meshIndex);

    CreateInfoCollection GetCreateInfoCollection();

    AnimatedGameObject* GetAnimatedGameObjectByObjectId(uint64_t objectID);
    Door* GetDoorByObjectId(uint64_t objectID);
    Door* GetDoorByDoorFrameObjectId(uint64_t objectID);

    GenericObject* GetGenericObjectById(uint64_t objectId);

    Piano* GetPianoByObjectId(uint64_t objectId);
    Piano* GetPianoByMeshNodeObjectId(uint64_t objectId);
    PianoKey* GetPianoKeyByObjectId(uint64_t objectId);
    PickUp* GetPickUpByObjectId(uint64_t objectID);
    PictureFrame* GetPictureFrameByObjectId(uint64_t objectId);
    Plane* GetPlaneByObjectId(uint64_t objectID); 
    Tree* GetTreeByObjectId(uint64_t objectId);
    Wall* GetWallByObjectId(uint64_t objectID);
    Wall* GetWallByWallSegmentObjectId(uint64_t objectID);
    Shark* GetSharkByObjectId(uint64_t objectID);
    Window* GetWindowByObjectId(uint64_t objectId);
    GameObject* GetGameObjectByIndex(int32_t index);
    GameObject* GetGameObjectByName(const std::string& name);
    Light* GetLightByIndex(int32_t index);
    PickUp* GetPickUpByIndex(int32_t index);
    Tree* GetTreeByIndex(int32_t index);

    size_t GetLightCount();

    std::vector<AnimatedGameObject>& GetAnimatedGameObjects();
    std::vector<ScreenSpaceBloodDecal>& GetScreenSpaceBloodDecals();
    std::vector<Bullet>& GetBullets();
    std::vector<BulletCasing>& GetBulletCasings();
    std::vector<ChristmasLights>& GetChristmasLights();
    std::vector<ChristmasPresent>& GetChristmasPresents();
    std::vector<ChristmasTree>& GetChristmasTrees();
    std::vector<ClippingCube>& GetClippingCubes();
    std::vector<Decal>& GetDecals();
    Hell::SlotMap<Door>& GetDoors();
    std::vector<Dobermann>& GetDobermanns();
    Hell::SlotMap<GenericObject>& GetGenericObjects();
    std::vector<Fence>& GetFences();
    std::vector<GameObject>& GetGameObjects();
    std::vector<GenericBouncable>& GetGenericBouncables();
    std::vector<GenericStatic>& GetGenericStatics();
    std::vector<HeightMapChunk>& GetHeightMapChunks();
    std::vector<Plane>& GetPlanes();
    std::vector<Light>& GetLights();
    std::vector<Kangaroo>& GetKangaroos();
    std::vector<MapInstance>& GetMapInstances();
    std::vector<Mermaid>& GetMermaids();
    std::vector<Piano>& GetPianos();
    std::vector<PickUp>& GetPickUps();
    std::vector<PictureFrame>& GetPictureFrames();
    std::vector<PowerPoleSet>& GetPowerPoleSets();
    std::vector<SpawnPoint>& GetCampaignSpawnPoints();
    std::vector<SpawnPoint>& GetDeathmatchSpawnPoints();
    std::vector<Transform>& GetDoorAndWindowCubeTransforms();
    std::vector<Road>& GetRoads();
    std::vector<Shark>& GetSharks();
    std::vector<Tree>& GetTrees();
    std::vector<Wall>& GetWalls();
    std::vector<VolumetricBloodSplatter>& GetVolumetricBloodSplatters();
    Hell::SlotMap<Window>& GetWindows();

    std::vector<RenderItem>& GetSkinnedRenderItems();
}