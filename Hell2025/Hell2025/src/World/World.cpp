#include "World.h"
#include "CreateInfo.h"
#include "HellDefines.h"
#include "HellLogging.h"
#include "HellTypes.h"
#include "UniqueID.h"
#include "Util.h"

#include "AssetManagement/AssetManager.h"
#include "Audio/Audio.h"
#include "Core/Game.h"
#include "Editor/Editor.h"
#include "Input/Input.h"
#include "Managers/MapManager.h"
#include "Renderer/GlobalIllumination.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderDataManager.h"
#include "Physics/Physics.h"
#include "World/HouseManager.h"
#include "World/SectorManager.h"

#include "Pathfinding/AStarMap.h"

#include "Physics/Types/Ragdoll.h"

namespace World {
    std::vector<AnimatedGameObject> g_animatedGameObjects;
    std::vector<ScreenSpaceBloodDecal> g_screenSpaceBloodDecals;
    std::vector<Bullet> g_bullets;
    std::vector<BulletCasing> g_bulletCasings;
    std::vector<ChristmasLights> g_christmasLights;
    std::vector<ChristmasPresent> g_christmasPresents;
    std::vector<ChristmasTree> g_christmasTrees;
    std::vector<ClippingCube> g_clippingCubes;
    std::vector<Door> g_doors;
    std::vector<Decal> g_decals;
    std::vector<Drawers> g_drawers;
    std::vector<GameObject> g_gameObjects;
    std::vector<GenericStatic> g_genericStatics;
    std::vector<GenericBouncable> g_genericBouncables;
    std::vector<Kangaroo> g_kangaroos;
    std::vector<HeightMapChunk> g_heightMapChunks;
    std::vector<Light> g_lights;
    std::vector<MapInstance> g_mapInstances;
    std::vector<Mermaid> g_mermaids;
    std::vector<Plane> g_planes;
    std::vector<PickUp> g_pickUps;
    std::vector<PictureFrame> g_pictureFrames;
    std::vector<Piano> g_pianos;
    std::vector<Road> g_roads;
    std::vector<Shark> g_sharks;
    std::vector<SpawnPoint> g_spawnCampaignPoints;
    std::vector<SpawnPoint> g_spawnDeathmatchPoints;
    std::vector<Toilet> g_toilets;
    std::vector<Transform> g_doorAndWindowCubeTransforms;
    std::vector<Tree> g_trees;
    std::vector<Wall> g_walls;
    std::vector<VolumetricBloodSplatter> g_volumetricBloodSplatters;
    std::vector<Window> g_windows;

    std::vector<GPULight> g_gpuLightsLowRes;
    std::vector<GPULight> g_gpuLightsMidRes;
    std::vector<GPULight> g_gpuLightsHighRes;

    std::map<ivecXZ, int> g_validChunks;

    std::string g_mapName = "";
    uint32_t g_worldMapChunkCountX = 0;
    uint32_t g_worldMapChunkCountZ = 0;

    std::vector<SpawnPoint> g_fallbackSpawnPoints = {
        SpawnPoint(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(-0.162, -HELL_PI * 0.5f, 0)),
        SpawnPoint(glm::vec3(1.5f, 0.0f, 2.0f), glm::vec3(-0.162, -HELL_PI * 0.5f, 0)),
        SpawnPoint(glm::vec3(3.0f, 0.0f, 2.0f), glm::vec3(-0.162, -HELL_PI * 0.5f, 0)),
        SpawnPoint(glm::vec3(4.5f, 0.0f, 2.0f), glm::vec3(-0.162, -HELL_PI * 0.5f, 0))
    };

    struct WorldState {
        bool oceanEnabled = true;
    } g_worldState;

    void AddSectorAtLocation(SectorCreateInfo& sectorCreateInfo, SpawnOffset spawnOffset, bool loadHouses);

    void Init() {
        KangarooCreateInfo kangarooCreateInfo;
        kangarooCreateInfo.position = glm::vec3(45, 32.6, 39);
        kangarooCreateInfo.rotation = glm::vec3(0, HELL_PI * -0.5f, 0);

        AddKangaroo(kangarooCreateInfo);

        NewRun();

        if (GetRoads().size() == 0) {
            Road& road = GetRoads().emplace_back();
            road.Init();
        }
    }

    void LoadMapInstance(const std::string& mapName) {
        ResetWorld();

        MapInstanceCreateInfo createInfo;
        createInfo.mapName = mapName;
        createInfo.spawnOffsetChunkX = 0;
        createInfo.spawnOffsetChunkZ = 0;

        LoadMapInstances({ createInfo });
    }

    void LoadMapInstances(std::vector<MapInstanceCreateInfo> mapInstanceCreateInfoSet) {
        g_mapInstances.clear();
        g_worldMapChunkCountX = 0;
        g_worldMapChunkCountZ = 0;

        // Load height map data from all map instances
        for (MapInstanceCreateInfo& mapInstanceCreateInfo : mapInstanceCreateInfoSet) {
            int32_t mapIndex = MapManager::GetMapIndexByName(mapInstanceCreateInfo.mapName);
            if (mapIndex == -1) {
                Logging::Error() << "World::LoadMapInstances() failed coz '" << mapInstanceCreateInfo.mapName << "' was not found";
                return;
            }

            Map* map = MapManager::GetMapByName(mapInstanceCreateInfo.mapName);

            MapInstance& mapInstance = g_mapInstances.emplace_back();
            mapInstance.m_mapIndex = mapIndex;
            mapInstance.spawnOffsetChunkX = mapInstanceCreateInfo.spawnOffsetChunkX;
            mapInstance.spawnOffsetChunkZ = mapInstanceCreateInfo.spawnOffsetChunkZ;

            uint32_t reachX = mapInstance.spawnOffsetChunkX + map->GetChunkCountX();
            uint32_t reachZ = mapInstance.spawnOffsetChunkZ + map->GetChunkCountZ();

            g_worldMapChunkCountX = std::max(g_worldMapChunkCountX, reachX);
            g_worldMapChunkCountZ = std::max(g_worldMapChunkCountZ, reachZ);

            // Add spawn points
            g_spawnCampaignPoints.insert(g_spawnCampaignPoints.end(), map->GetAdditionalMapData().playerCampaignSpawns.begin(), map->GetAdditionalMapData().playerCampaignSpawns.end());
            g_spawnDeathmatchPoints.insert(g_spawnDeathmatchPoints.end(), map->GetAdditionalMapData().playerDeathmatchSpawns.begin(), map->GetAdditionalMapData().playerDeathmatchSpawns.end());
        }

        // Create heightmap chunks
        g_heightMapChunks.clear();
        g_validChunks.clear();

        // Init heightmap chunks
        int baseVertex = 0;
        int baseIndex = 0;
        for (int x = 0; x < g_worldMapChunkCountX; x++) {
            for (int z = 0; z < g_worldMapChunkCountZ; z++) {
                int cellX = x / 8;
                int cellZ = z / 8;

                HeightMapChunk& chunk = g_heightMapChunks.emplace_back();
                chunk.coord.x = x;
                chunk.coord.z = z;
                chunk.baseVertex = baseVertex;
                chunk.baseIndex = baseIndex;
                baseVertex += VERTICES_PER_CHUNK;
                baseIndex += INDICES_PER_CHUNK;

                g_validChunks[chunk.coord] = g_heightMapChunks.size() - 1;
            }
        }

        Renderer::RecalculateAllHeightMapData(true);

        // Load the objects from all map instances
        for (MapInstanceCreateInfo& mapInstanceCreateInfo : mapInstanceCreateInfoSet) {
            Map* map = MapManager::GetMapByName(mapInstanceCreateInfo.mapName);
            if (!map) {
                Logging::Error() << "World::LoadMapInstances() failed coz '" << mapInstanceCreateInfo.mapName << "' was not found";
                return;
            }
            SpawnOffset spawnOffset;
            spawnOffset.translation.x = mapInstanceCreateInfo.spawnOffsetChunkX * HEIGHT_MAP_CHUNK_WORLD_SPACE_SIZE;
            spawnOffset.translation.z = mapInstanceCreateInfo.spawnOffsetChunkZ * HEIGHT_MAP_CHUNK_WORLD_SPACE_SIZE;
            LoadMapInstanceObjects(mapInstanceCreateInfo.mapName, spawnOffset);

            SpawnOffset houseSpawnOffset = spawnOffset;
            houseSpawnOffset.translation += glm::vec3(34.0f, 31.0f, 36.0f);

            HouseCreateInfo* houseCreateInfo = HouseManager::GetHouseCreateInfoByFilename("TestHouse");
            World::AddHouse(*houseCreateInfo, houseSpawnOffset);
        }

        GameObjectCreateInfo createInfo2;
        createInfo2.position = glm::vec3(32.0f, 30.4f, 38.25f);
        createInfo2.scale = glm::vec3(1.0f);
        createInfo2.modelName = "RuralSet";
        AddGameObject(createInfo2);
        g_gameObjects[0].m_meshNodes.SetMeshMaterials("RuralSet0");


        createInfo2.position = glm::vec3(21.0f, 30.4f, 14.25f);
        createInfo2.scale = glm::vec3(1.0f);
        createInfo2.modelName = "RuralSet";
        AddGameObject(createInfo2);
        g_gameObjects[1].m_meshNodes.SetMeshMaterials("RuralSet0");

        // Init all spawn points (creates the physics shapes you need to detect hover in the editor)
        for (SpawnPoint& spawnPoint : g_spawnCampaignPoints) {
            spawnPoint.Init();
        }
        for (SpawnPoint& spawnPoint : g_spawnDeathmatchPoints) {
            spawnPoint.Init();
        }
    }

    void AddCreateInfoCollection(CreateInfoCollection& createInfoCollection, SpawnOffset spawnOffset) {
        for (PickUpCreateInfo& createInfo : createInfoCollection.pickUps) {
            AddPickUp(createInfo, spawnOffset);
        }
        for (TreeCreateInfo& createInfo : createInfoCollection.trees) {
            AddTree(createInfo, spawnOffset);
        }
    }

    CreateInfoCollection GetCreateInfoCollection() {
        CreateInfoCollection createInfoCollection;

        for (PickUp& pickUp : World::GetPickUps()) {
            createInfoCollection.pickUps.push_back(pickUp.GetCreateInfo());
        }
        for (Tree& tree : World::GetTrees()) {
            createInfoCollection.trees.push_back(tree.GetCreateInfo());
        }

        return createInfoCollection;
    }

    void LoadMapInstanceObjects(const std::string& mapName, SpawnOffset spawnOffset) {
        Map* map = MapManager::GetMapByName(mapName);
        if (!map) {
            Logging::Error() << "World::LoadMapInstanceObjects() failed coz '" << mapName << "' was not found";
            return;
        }
        AddCreateInfoCollection(map->GetCreateInfoCollection(), spawnOffset);
    }

    void NewRun() {
        //std::string sectorName = "TestSector";
        //SectorCreateInfo* sectorCreateInfo = SectorManager::GetSectorCreateInfoByName(sectorName);
        //if (sectorCreateInfo) {
        //    World::LoadSingleSector(sectorCreateInfo, true);
        //}

        Game::RespawnPlayers();

        // Add shark
        Shark& shark = g_sharks.emplace_back();
        shark.Init(glm::vec3(5.0f, 29.05f, 40.0f));

        // Respawn roos
        for (Kangaroo& kangaroo : g_kangaroos) {
            kangaroo.Respawn();
        }

        // Load two instances of the map
        std::vector<MapInstanceCreateInfo> mapInstanceCreateInfoSet;

        MapInstanceCreateInfo mapInstanceCreateInfo;
        mapInstanceCreateInfo.mapName = "Shit";
        mapInstanceCreateInfo.spawnOffsetChunkX = 0;
        mapInstanceCreateInfo.spawnOffsetChunkZ = 0;
        mapInstanceCreateInfoSet.push_back(mapInstanceCreateInfo);

        mapInstanceCreateInfo.mapName = "Shit";
        mapInstanceCreateInfo.spawnOffsetChunkX = 8;
        mapInstanceCreateInfo.spawnOffsetChunkZ = 4;
        mapInstanceCreateInfoSet.push_back(mapInstanceCreateInfo);

        LoadMapInstances(mapInstanceCreateInfoSet);

        Editor::SetEditorMapName(UNDEFINED_STRING);
    }

    void BeginFrame() {
        //RemoveAnyObjectMarkedForRemoval();

        for (GameObject& gameObject : g_gameObjects) {
            gameObject.BeginFrame();
        }
        for (Tree& tree : g_trees) {
            tree.BeginFrame();
        }
    }

    void EndFrame() {
        // Nothing as of yet
    }


    void CreateGameObject() {
        g_gameObjects.emplace_back();
    }

    uint64_t CreateAnimatedGameObject() {
        AnimatedGameObject& animatedGameObject = g_animatedGameObjects.emplace_back();
        animatedGameObject.Init();
        return animatedGameObject.GetObjectId();
    }

    AnimatedGameObject* GetAnimatedGameObjectByIndex(int32_t index) {
        if (index >= 0 && index < g_animatedGameObjects.size()) {
            return &g_animatedGameObjects[index];
        }
        else {
            return nullptr;
        }
    }

    GameObject* GetGameObjectByName(const std::string& name) {
        for (GameObject& gameObject : g_gameObjects) {
            if (gameObject.m_name == name) {
                return &gameObject;
            }
        }
        return nullptr;
    }

    GameObject* GetGameObjectByIndex(int32_t index) {
        if (index >= 0 && index < g_gameObjects.size()) {
            return &g_gameObjects[index];
        }
        else {
            return nullptr;
        }
    }

    Light* GetLightByIndex(int32_t index) {
        if (index >= 0 && index < g_lights.size()) {
            return &g_lights[index];
        }
        else {
            std::cout << "World::GetLightByIndex() failed: index " << index << " out of range of size " << g_lights.size() << "\n";
            return nullptr;
        }
    }

    PickUp* GetPickUpByIndex(int32_t index) {
        if (index >= 0 && index < g_pickUps.size()) {
            return &g_pickUps[index];
        }
        else {
            return nullptr;
        }
    }

    Tree* GetTreeByIndex(int32_t index) {
        if (index >= 0 && index < g_trees.size()) {
            return &g_trees[index];
        }
        else {
            return nullptr;
        }
    }

    void LoadMap(MapCreateInfo* mapCreateInfo) {
        //// // Handle failed map load
        //// if (!mapCreateInfo) {
        ////     std::cout << "World::LoadMap() failed: mapCreateInfo was nullptr\n";
        ////     return;
        //// }
        //// 
        //// // It loaded successfully so reset the world...
        //// ResetWorld();
        //// 
        //// g_mapName = mapCreateInfo->name;
        //// std::string sectorName = mapCreateInfo->m_sectorNames[0][0];
        //// SectorCreateInfo* sectorCreateInfo = SectorManager::GetSectorCreateInfoByName(sectorName);
        //// if (sectorCreateInfo) {
        ////     g_heightMapNames[0][0] = sectorCreateInfo->heightMapName;
        //// 
        ////     SpawnOffset spawnOffset;
        ////     spawnOffset.translation.x = SECTOR_SIZE_WORLD_SPACE;
        ////     spawnOffset.translation.z = SECTOR_SIZE_WORLD_SPACE;
        ////     AddSectorAtLocation(*sectorCreateInfo, spawnOffset, true);
        //// }
        //// 
        //// std::cout << "Loaded map: " << g_mapName << "\n";
    }

    void LoadSingleSector(SectorCreateInfo* sectorCreateInfo, bool loadHouses) {
        ///// if (!sectorCreateInfo) return;
        ///// 
        ///// ResetWorld();
        ///// 
        ///// g_mapName = "SectorEditorMap";
        ///// g_sectorNames[0][0] = sectorCreateInfo->sectorName;
        ///// g_heightMapNames[0][0] = sectorCreateInfo->heightMapName;
        ///// //g_mapWidth = 1;
        ///// //g_mapDepth = 1;
        ///// 
        ///// AddSectorAtLocation(*sectorCreateInfo, SpawnOffset(), loadHouses);
        ///// 
        ///// 
        ///// std::cout << "Loaded Single Sector: '" << g_sectorNames[0][0] << "' with height map '" << g_heightMapNames[0][0] << "'\n";
        ///// 
        ///// // TEST REMOVE ME!
        ///// g_gameObjects.clear();
        ///// 
        ///// GameObjectCreateInfo createInfo2;
        ///// createInfo2.position = glm::vec3(22.0f, 30.5f, 38.25f);
        ///// createInfo2.scale = glm::vec3(1.0f);
        ///// createInfo2.modelName = "Bunny";
        ///// AddGameObject(createInfo2);
        ///// g_gameObjects[0].m_meshNodes.m_materialIndices[0] = AssetManager::GetMaterialIndexByName("Leopard");
        ///// g_gameObjects[0].m_meshNodes.m_materialIndices[1] = AssetManager::GetMaterialIndexByName("Leopard");
        ///// 
        ///// 
        ///// //createInfo2.position = glm::vec3(30.0f, 30.3f, 38.25f);
        ///// //createInfo2.scale = glm::vec3(1.0f);
        ///// //createInfo2.modelName = "PowerPole";
        ///// //AddGameObject(createInfo2);
        ///// //
        ///// //
        ///// //createInfo2.position = glm::vec3(32.0f, 30.4f, 38.25f);
        ///// //createInfo2.scale = glm::vec3(1.0f);
        ///// //createInfo2.modelName = "Reflector";
        ///// //AddGameObject(createInfo2);
        ///// //
        ///// //createInfo2.position = glm::vec3(32.0f, 30.4f, 38.25f);
        ///// //createInfo2.scale = glm::vec3(1.0f);
        ///// //createInfo2.modelName = "Fence";
        ///// //AddGameObject(createInfo2);

    }

    void LoadSingleHouse(HouseCreateInfo* houseCreateInfo) {
        if (!houseCreateInfo) {
            std::cout << "World::LoadSingleHouse() failed: houseCreateInfo was nullptr\n";
            return;
        }

        ResetWorld();

        g_mapName = "HouseEditorMap";
        //g_mapWidth = 1;
        //g_mapDepth = 1;

        AddHouse(*houseCreateInfo, SpawnOffset());

        GlobalIllumination::SetGlobalIlluminationStructuresDirtyState(true);
    }

    void AddSectorAtLocation(SectorCreateInfo& sectorCreateInfo, SpawnOffset spawnOffset, bool loadHouses) {
        for (LightCreateInfo& createInfo : sectorCreateInfo.lights) {
            AddLight(createInfo, spawnOffset);
        }
        for (GameObjectCreateInfo& createInfo : sectorCreateInfo.gameObjects) {
            AddGameObject(createInfo, spawnOffset);
        }
        for (PickUpCreateInfo& createInfo : sectorCreateInfo.pickUps) {
            AddPickUp(createInfo, spawnOffset);
        }
        for (TreeCreateInfo& createInfo : sectorCreateInfo.trees) {
            AddTree(createInfo, spawnOffset);
        }

        if (loadHouses) {
            HouseManager::LoadAllHouseFilesFromDisk();

            glm::vec3 houseLocation = glm::vec3(15.0f, 30.5f, 40.0f);

            SpawnOffset houseSpawnOffset = spawnOffset;
            houseSpawnOffset.translation += houseLocation;

            HouseCreateInfo* houseCreateInfo = HouseManager::GetHouseCreateInfoByFilename("TestHouse");
            if (houseCreateInfo) {
                AddHouse(*houseCreateInfo, houseSpawnOffset);
            }

            GlobalIllumination::SetGlobalIlluminationStructuresDirtyState(true);
        }
    }

    AnimatedGameObject* GetAnimatedGameObjectByObjectId(uint64_t objectID) {
        for (AnimatedGameObject& animatedGameObject : g_animatedGameObjects) {
            if (animatedGameObject.GetObjectId() == objectID) {
                return &animatedGameObject;
            }
        }
        return nullptr;
    }

    Door* GetDoorByObjectId(uint64_t objectID) {
        for (int i = 0; i < g_doors.size(); i++) {
            Door& door = g_doors[i];
            if (door.GetObjectId() == objectID) {
                return &g_doors[i];
            }
        }
        return nullptr;
    }

    Door* GetDoorByDoorFrameObjectId(uint64_t objectID) {
        for (int i = 0; i < g_doors.size(); i++) {
            Door& door = g_doors[i];
            if (door.GetFrameObjectId() == objectID) {
                return &g_doors[i];
            }
        }
        return nullptr;
    }

    PickUp* GetPickUpByObjectId(uint64_t objectID) {
        for (int i = 0; i < g_pickUps.size(); i++) {
            PickUp& pickUp = g_pickUps[i];
            if (pickUp.GetObjectId() == objectID) {
                return &g_pickUps[i];
            }
        }
        return nullptr;
    }

    PianoKey* GetPianoKeyByObjectId(uint64_t objectId) {
        for (Piano& piano : World::GetPianos()) {
            if (piano.PianoKeyExists(objectId)) {
                return piano.GetPianoKey(objectId);
            }
        }
        return nullptr;
    }

    PictureFrame* GetPictureFrameByObjectId(uint64_t objectId) {
        for (PictureFrame& pictureFrame : World::GetPictureFrames()) {
            if (pictureFrame.GetObjectId() == objectId) {
                return &pictureFrame;
            }
        }
        return nullptr;
    }

    Plane* GetPlaneByObjectId(uint64_t objectID) {
        for (int i = 0; i < g_planes.size(); i++) {
            Plane& plane = g_planes[i];
            if (plane.GetObjectId() == objectID) {
                return &g_planes[i];
            }
        }
        return nullptr;
    }

    Wall* GetWallByObjectId(uint64_t objectID) {
        for (int i = 0; i < g_walls.size(); i++) {
            Wall& wall = g_walls[i];
            if (wall.GetObjectId() == objectID) {
                return &g_walls[i];
            }
        }
        return nullptr;
    }

    Wall* GetWallByWallSegmentObjectId(uint64_t objectId) {
        for (Wall& wall : g_walls) {
            for (WallSegment& wallSegment : wall.GetWallSegments()) {
                if (wallSegment.GetObjectId() == objectId) {
                    return &wall;
                }
            }
        }
        return nullptr;
    }    

    Toilet* GetToiletByMeshNodeObjectId(uint64_t objectId) {
        for (Toilet& toilet : g_toilets) {
            const MeshNodes& meshNodes = toilet.GetMeshNodes();
            if (meshNodes.HasNodeWithObjectId(objectId)) {
                return &toilet;
            }
        }
        return nullptr;
    }

    Piano* GetPianoByMeshNodeObjectId(uint64_t objectId) {
        for (Piano& piano : g_pianos) {

            const MeshNodes& meshNodes = piano.GetMeshNodes();
            if (meshNodes.HasNodeWithObjectId(objectId)) {
                return &piano;
            }
        }
        return nullptr;
    }

    void SetObjectPosition(uint64_t objectId, glm::vec3 position) {
        Door* door = World::GetDoorByObjectId(objectId);
        if (door) {
            door->SetPosition(position);
            UpdateClippingCubes();
            UpdateAllWallCSG();
            UpdateHouseMeshBuffer();
            UpdateWeatherBoardMeshBuffer();
            Physics::ForceZeroStepUpdate();
        }

        Piano* piano = World::GetPianoByObjectId(objectId);
        if (piano) {
            piano->SetPosition(position);
            Physics::ForceZeroStepUpdate();
        }

        Plane* plane = World::GetPlaneByObjectId(objectId);
        if (plane) {
            plane->UpdateWorldSpaceCenter(position);
            UpdateHouseMeshBuffer();
            UpdateWeatherBoardMeshBuffer();
        }

        PictureFrame* pictureFrame = World::GetPictureFrameByObjectId(objectId);
        if (pictureFrame) {
            pictureFrame->SetPosition(position);
        }

        Tree* tree = World::GetTreeByObjectId(objectId);
        if (tree) {
            tree->SetPosition(position);
        }

        Wall* wall = World::GetWallByObjectId(objectId);
        if (wall) {
            wall->UpdateWorldSpaceCenter(position);
            Physics::ForceZeroStepUpdate();
            UpdateHouseMeshBuffer();
            UpdateWeatherBoardMeshBuffer();
        }

        Window* window = World::GetWindowByObjectId(objectId);
        if (window) {
            window->SetPosition(position);
            UpdateClippingCubes();
            UpdateAllWallCSG();
            UpdateHouseMeshBuffer();
            UpdateWeatherBoardMeshBuffer();
            Physics::ForceZeroStepUpdate();
        }
    }

    void RemoveObject(uint64_t objectID) {
        for (int i = 0; i < g_doors.size(); i++) {
            if (g_doors[i].GetObjectId() == objectID) {
                g_doors[i].CleanUp();
                g_doors.erase(g_doors.begin() + i);
                return;
            }
        }
        for (int i = 0; i < g_pianos.size(); i++) {
            if (g_pianos[i].GetObjectId() == objectID) {
                g_pianos[i].CleanUp();
                g_pianos.erase(g_pianos.begin() + i);
                return;
            }
        }
        for (int i = 0; i < g_planes.size(); i++) {
            if (g_planes[i].GetObjectId() == objectID) {
                g_planes[i].CleanUp();
                g_planes.erase(g_planes.begin() + i);
                return;
            }
        }
        for (int i = 0; i < g_pickUps.size(); i++) {
            if (g_pickUps[i].GetObjectId() == objectID) {
                g_pickUps[i].CleanUp();
                g_pickUps.erase(g_pickUps.begin() + i);
                return;
            }
        }
        for (int i = 0; i < g_walls.size(); i++) {
            if (g_walls[i].GetObjectId() == objectID) {
                g_walls[i].CleanUp();
                g_walls.erase(g_walls.begin() + i);
                return;
            }
        }
        for (int i = 0; i < g_windows.size(); i++) {
            if (g_windows[i].GetObjectId() == objectID) {
                g_windows[i].CleanUp();
                g_windows.erase(g_windows.begin() + i);
                return;
            }
        }

        for (int i = 0; i < g_animatedGameObjects.size(); i++) {
            if (g_animatedGameObjects[i].GetObjectId() == objectID) {
                g_animatedGameObjects[i].CleanUp();
                g_animatedGameObjects.erase(g_animatedGameObjects.begin() + i);
                return;
            }
        }

        for (int i = 0; i < g_trees.size(); i++) {
            if (g_trees[i].GetObjectId() == objectID) {
                Logging::Debug() << "Deleted " << g_trees[i].GetEditorName();
                g_trees[i].CleanUp();
                g_trees.erase(g_trees.begin() + i);
                return;
            }
        }

        Logging::Error() << "World::RemoveObject() Failed to remove object " << objectID << ", check you have implemented this type!\n";
    }

    void ResetWorld() {
        std::cout << "Reset world()\n";

        // Clear heightmap data
        g_heightMapChunks.clear();
        g_validChunks.clear();
        g_mapInstances.clear();

        ResetWeatherboardMeshBuffer();

        // Cleanup all objects
        ClearAllObjects();

        MermaidCreateInfo mermaidCreateInfo;
        mermaidCreateInfo.position = glm::vec3(29.0f, 29.5f, 52.5f);
        mermaidCreateInfo.rotation.y = 0.25f;
        AddMermaid(mermaidCreateInfo);

        //AnimatedGameObject* animatedGameObject = nullptr;
        //uint64_t id = World::CreateAnimatedGameObject();
        //animatedGameObject = World::GetAnimatedGameObjectByObjectId(id);
        //animatedGameObject->SetSkinnedModel("Glock");
        //animatedGameObject->SetName("Remington870");
        //animatedGameObject->SetAllMeshMaterials("Glock");
        //animatedGameObject->PlayAndLoopAnimation("MainLayer", "Glock_Reload", 1.0f);
        //animatedGameObject->SetScale(0.01);
        //animatedGameObject->SetPosition(glm::vec3(17, 31, 40));
        //animatedGameObject->SetMeshMaterialByMeshName("ArmsMale", "Hands");
        //animatedGameObject->SetMeshMaterialByMeshName("ArmsFemale", "FemaleArms");
    }

    void ClearAllObjects() {
        // Clean up
        for (BulletCasing& bulletCasing : g_bulletCasings)              bulletCasing.CleanUp();
        for (ChristmasLights& christmasLights : g_christmasLights)      christmasLights.CleanUp();
        for (ChristmasPresent& christmasPresent : g_christmasPresents)  christmasPresent.CleanUp();
        for (ChristmasTree& christmasTree : g_christmasTrees)           christmasTree.CleanUp();
        for (Door& door : g_doors)                                      door.CleanUp();
        for (GameObject& gameObject : g_gameObjects)                    gameObject.CleanUp();
        //for (GenericBouncable& genericBouncable : g_genericBouncables) genericStatic.CleanUp();
        for (GenericStatic& genericStatic : g_genericStatics)           genericStatic.CleanUp();
        //for (Kangaroo& kangaroo : g_kangaroos)                        kangaroo.CleanUp();
        for (Mermaid& mermaid : g_mermaids)                             mermaid.CleanUp();
        for (Plane& housePlane : g_planes)                              housePlane.CleanUp();
        for (Piano& piano : g_pianos)                                   piano.CleanUp();
        for (PickUp& pickUp : g_pickUps)                                pickUp.CleanUp();
        for (Shark& shark : g_sharks)                                   shark.CleanUp();
        for (SpawnPoint& spawnPoint : g_spawnCampaignPoints)            spawnPoint.CleanUp();
        for (SpawnPoint& spawnPoint : g_spawnDeathmatchPoints)          spawnPoint.CleanUp();
        for (Tree& tree : g_trees)                                      tree.CleanUp();
        for (Wall& wall : g_walls)                                      wall.CleanUp();
        for (Window& window : g_windows)                                window.CleanUp();
        
        // Clear all containers
        g_bulletCasings.clear();
        g_screenSpaceBloodDecals.clear();
        g_christmasLights.clear();
        g_christmasPresents.clear();
        g_christmasTrees.clear();
        g_decals.clear();
        g_doors.clear();
        g_gameObjects.clear();
        g_genericStatics.clear();
        //g_kangaroos.clear();
        g_lights.clear();
        g_mermaids.clear();
        g_pianos.clear();
        g_pickUps.clear();
        g_planes.clear();
        g_pictureFrames.clear();
        g_sharks.clear();
        g_spawnCampaignPoints.clear();
        g_spawnDeathmatchPoints.clear();
        g_toilets.clear();
        g_trees.clear();
        g_walls.clear();
        g_windows.clear();
    }

    void UpdateClippingCubes() {
        g_clippingCubes.clear();
        for (Door& door : g_doors) {
            Transform transform;
            transform.position = door.GetPosition();
            transform.position.y += DOOR_HEIGHT * 0.5f;
            transform.rotation = door.GetRotation();
            transform.scale = glm::vec3(0.2f, DOOR_HEIGHT * 1.01f, DOOR_WIDTH);

            ClippingCube& cube = g_clippingCubes.emplace_back();
            cube.Update(transform);
        }

        for (Window& window : g_windows) {
            Transform transform;
            transform.position = window.GetPosition();
            transform.position.y += 1.5f;
            transform.rotation = window.GetRotation();
            transform.scale = glm::vec3(0.2f, 1.185074f, 0.76f);

            ClippingCube& cube = g_clippingCubes.emplace_back();
            cube.Update(transform);
        }
    }

    void UpdateAllWallCSG() {
        for (Wall& wall : GetWalls()) {
            wall.UpdateSegmentsAndVertexData();
        }
    }

    void AddBullet(BulletCreateInfo createInfo) {
        g_bullets.push_back(Bullet(createInfo));
    }

    void AddDoor(DoorCreateInfo createInfo, SpawnOffset spawnOffset) {
        Door& door = g_doors.emplace_back();
        createInfo.position += spawnOffset.translation;
        door.Init(createInfo);
    }

    void AddBulletCasing(BulletCasingCreateInfo createInfo, SpawnOffset spawnOffset) {
        createInfo.position += spawnOffset.translation;
        g_bulletCasings.push_back(BulletCasing(createInfo));
    }

    void AddDecal(const DecalCreateInfo& createInfo) {
        Decal& decal = g_decals.emplace_back();
        decal.Init(createInfo);
    }

    void AddKangaroo(const KangarooCreateInfo& createInfo) {
        Kangaroo& kangaroo = g_kangaroos.emplace_back();
        kangaroo.Init(createInfo);        
    }

    void AddChristmasLights(ChristmasLightsCreateInfo createInfo, SpawnOffset spawnOffset) {
        g_christmasLights.push_back(ChristmasLights(createInfo, spawnOffset));
    }

    void AddChristmasPresent(ChristmasPresentCreateInfo createInfo, SpawnOffset spawnOffset) {
        g_christmasPresents.push_back(ChristmasPresent(createInfo, spawnOffset));
    }

    void AddChristmasTree(ChristmasTreeCreateInfo createInfo, SpawnOffset spawnOffset) {
        g_christmasTrees.push_back(ChristmasTree(createInfo, spawnOffset));
    }

    void AddHousePlane(PlaneCreateInfo createInfo, SpawnOffset spawnOffset) {
        createInfo.p0 += spawnOffset.translation;
        createInfo.p1 += spawnOffset.translation;
        createInfo.p2 += spawnOffset.translation;
        createInfo.p3 += spawnOffset.translation;
        Plane& housePlane = g_planes.emplace_back();
        housePlane.Init(createInfo);
        //return housePlane.GetObjectId();
    }

    void AddGameObject(GameObjectCreateInfo createInfo, SpawnOffset spawnOffset) {
        createInfo.position += spawnOffset.translation;
        g_gameObjects.push_back(GameObject(createInfo));
    }

    void AddGenericBouncable(GenericBouncableCreateInfo createInfo, SpawnOffset spawnOffset) {
        g_genericBouncables.push_back(GenericBouncable(createInfo, spawnOffset));
    }

    void AddGenericStatic(GenericStaticCreateInfo createInfo, SpawnOffset spawnOffset) {
        g_genericStatics.push_back(GenericStatic(createInfo, spawnOffset));
    }

    void AddLight(LightCreateInfo createInfo, SpawnOffset spawnOffset) {
        createInfo.position += spawnOffset.translation;
        g_lights.push_back(Light(createInfo));
    }

    void AddMermaid(MermaidCreateInfo createInfo, SpawnOffset spawnOffset) {
        Mermaid& mermaid = g_mermaids.emplace_back();
        mermaid.Init(createInfo, spawnOffset);
    }

    void AddToilet(ToiletCreateInfo createInfo, SpawnOffset spawnOffset) {
        g_toilets.push_back(Toilet(createInfo, spawnOffset));
    }

    void AddDrawers(const DrawersCreateInfo& createInfo, SpawnOffset spawnOffset) {
        g_drawers.push_back(Drawers(createInfo, spawnOffset));
    }

    void AddScreenSpaceBloodDecal(ScreenSpaceBloodDecalCreateInfo createInfo) {
        ScreenSpaceBloodDecal& screenSpaceBloodDecal = g_screenSpaceBloodDecals.emplace_back();
        screenSpaceBloodDecal.Init(createInfo);
    }

    void AddPiano(PianoCreateInfo createInfo, SpawnOffset spawnOffset) {
        createInfo.position += spawnOffset.translation;
        Piano& piano = g_pianos.emplace_back();
        piano.Init(createInfo);
    }

    void AddPickUp(PickUpCreateInfo createInfo, SpawnOffset spawnOffset) {
        createInfo.position += spawnOffset.translation;

        PickUp& pickUp = g_pickUps.emplace_back();
        pickUp.Init(createInfo);
    }

    void AddPictureFrame(PictureFrameCreateInfo createInfo, SpawnOffset spawnOffset) {
        createInfo.position += spawnOffset.translation;

        PictureFrame& pictureFrame = g_pictureFrames.emplace_back();
        pictureFrame.Init(createInfo);
    }

    void AddTree(TreeCreateInfo createInfo, SpawnOffset spawnOffset) {
        createInfo.position += spawnOffset.translation;
        if (createInfo.editorName == UNDEFINED_STRING) {
            createInfo.editorName = Editor::GetNextAvailableTreeName(createInfo.type);
        }
        g_trees.push_back(Tree(createInfo));
    }

    void AddVATBlood(glm::vec3 position, glm::vec3 front) {
        int maxAllowed = 4;
        if (g_volumetricBloodSplatters.size() < maxAllowed) {
            g_volumetricBloodSplatters.push_back(VolumetricBloodSplatter(position, front));
        }
    }

    uint64_t AddWall(WallCreateInfo createInfo, SpawnOffset spawnOffset) {
        if (createInfo.points.empty()) {
            std::cout << "World::AddWall() failed: createInfo has zero points!\n";
            return 0;
        }

        for (glm::vec3& point : createInfo.points) {
            point += spawnOffset.translation;
        }

        Wall& wall = g_walls.emplace_back();
        wall.Init(createInfo, spawnOffset);

        return wall.GetObjectId();
    }

    SpawnPoint GetRandomCampaignSpawnPoint() {
        SpawnPoint spawnPoint;
        if (g_spawnCampaignPoints.size()) {
            int rand = Util::RandomInt(0, g_spawnCampaignPoints.size() - 1); g_spawnCampaignPoints[rand];
            spawnPoint = g_spawnCampaignPoints[rand];
        }
        else {
            int rand = Util::RandomInt(0, g_fallbackSpawnPoints.size() - 1); g_fallbackSpawnPoints[rand];
            spawnPoint = g_fallbackSpawnPoints[rand];
        }

        // Check you didn't just spawn on another player
        for (int i = 0; i < Game::GetLocalPlayerCount(); i++) {
            Player* player = Game::GetLocalPlayerByIndex(i);
            float distanceToOtherPlayer = glm::distance(spawnPoint.GetPosition(), player->GetFootPosition());
            if (distanceToOtherPlayer < 1.0f) {
                return GetRandomCampaignSpawnPoint();
            }
        }

        return spawnPoint;
    }

    SpawnPoint GetRandomDeathmanSpawnPoint() {
        SpawnPoint spawnPoint;
        if (g_spawnDeathmatchPoints.size()) {
            int rand = Util::RandomInt(0, g_spawnDeathmatchPoints.size() - 1); g_spawnDeathmatchPoints[rand];
            spawnPoint = g_spawnDeathmatchPoints[rand];
        }
        else {
            int rand = Util::RandomInt(0, g_fallbackSpawnPoints.size() - 1); g_fallbackSpawnPoints[rand];
            spawnPoint = g_fallbackSpawnPoints[rand];
        }

        // Check you didn't just spawn on another player
        for (int i = 0; i < Game::GetLocalPlayerCount(); i++) {
            Player* player = Game::GetLocalPlayerByIndex(i);
            float distanceToOtherPlayer = glm::distance(spawnPoint.GetPosition(), player->GetFootPosition());
            if (distanceToOtherPlayer < 1.0f) {
                return GetRandomCampaignSpawnPoint();
            }
        }

        return spawnPoint;
    }

    void UpdateWorldSpawnPointsFromMap(Map* map) {
        if (!map) {
            Logging::Error() << "World::UpdateWorldSpawnPointsFromMap() failed coz map param was nullptr";
            return;
        }
        g_spawnCampaignPoints = map->GetAdditionalMapData().playerCampaignSpawns;
        g_spawnDeathmatchPoints = map->GetAdditionalMapData().playerDeathmatchSpawns;
    }

    void EnableOcean() {
        g_worldState.oceanEnabled = true;
    }

    void DisableOcean() {
        g_worldState.oceanEnabled = false;
    }

    bool HasOcean() {
        return g_worldState.oceanEnabled;
    }

    void AddWindow(WindowCreateInfo createInfo, SpawnOffset spawnOffset) {
        Window& window = g_windows.emplace_back();
        createInfo.position += spawnOffset.translation;
        window.Init(createInfo);
    }

    void AddMapInstance(const std::string& mapName, int32_t spawnOffsetChunkX, int32_t spawnOffsetChunkZ) {

    }

    std::vector<HeightMapChunk>& GetHeightMapChunks() {
        return g_heightMapChunks;
    }

    const uint32_t GetChunkCountX() {
        return g_worldMapChunkCountX;
    }

    const uint32_t GetChunkCountZ() {
        return g_worldMapChunkCountZ;
    }

    const uint32_t GetChunkCount() {
        return (uint32_t)g_heightMapChunks.size();
    }

    bool ChunkExists(int x, int z) {
        return g_validChunks.contains(ivecXZ(x, z));
    }

    const HeightMapChunk* GetChunk(int x, int z) {
        if (!ChunkExists(x, z)) return nullptr;

        int index = g_validChunks[ivecXZ(x, z)];
        return &g_heightMapChunks[index];
    }

    const std::string& GetCurrentMapName() {
        return g_mapName;
    }


    const float GetWorldSpaceWidth() {
        return g_worldMapChunkCountX * HEIGHT_MAP_CHUNK_WORLD_SPACE_SIZE;
    }

    const float GetWorldSpaceDepth() {
        return g_worldMapChunkCountZ * HEIGHT_MAP_CHUNK_WORLD_SPACE_SIZE;
    }

    ///// const std::string& GetSectorNameAtLocation(int x, int z) {
    /////     if (x < 0 || x >= 1 || z < 0 || z >= 1) {
    /////         std::cout << "World::GetSectorNameAtLocation() failed: [" << x << "][" << z << "] out of range of size [" << 1 << "][" << 1 << "]\n";
    /////         static const std::string emptyStr = "";
    /////         return emptyStr;
    /////     }
    /////     return g_sectorNames[x][z];
    ///// }
    ///// 
    ///// const std::string& GetHeightMapNameAtLocation(int x, int z) {
    /////     if (x < 0 || x >= 1 || z < 0 || z >= 1) {
    /////         std::cout << "World::GetHeightMapNameAtLocation() failed: [" << x << "][" << z << "] out of range of size [" << 1 << "][" << 1 << "]\n";
    /////         static const std::string emptyStr = "";
    /////         return emptyStr;
    /////     }
    /////     return g_heightMapNames[x][z];
    ///// }

    ///// bool IsMapCellInRange(int x, int z) {
    /////     return (x >= 0 && x < 1 && z >= 0 && z < 1);
    ///// }

    Piano* GetPianoByObjectId(uint64_t objectId) {
        for (Piano& piano : g_pianos) {
            if (piano.GetObjectId() == objectId) {
                return &piano;
            }
        }
        return nullptr;
    }

    Shark* GetSharkByObjectId(uint64_t objectId) {
        for (Shark& shark: g_sharks) {
            if (shark.GetObjectId() == objectId) {
                return &shark;
            }
        }
        return nullptr;
    }


    Tree* GetTreeByObjectId(uint64_t objectId) {
        for (Tree& tree : g_trees) {
            if (tree.GetObjectId() == objectId) {
                return &tree;
            }
        }
        return nullptr;
    }
    Window* GetWindowByObjectId(uint64_t objectId) {
        for (Window& window : g_windows) {
            if (window.GetObjectId() == objectId) {
                return &window;
            }
        }
        return nullptr;
    }

    size_t GetLightCount()                                              { return g_lights.size(); }

    std::vector<AnimatedGameObject>& GetAnimatedGameObjects()           { return g_animatedGameObjects; }
    std::vector<ScreenSpaceBloodDecal>& GetScreenSpaceBloodDecals()     { return g_screenSpaceBloodDecals; }
    std::vector<Bullet>& GetBullets()                                   { return g_bullets; }
    std::vector<BulletCasing>& GetBulletCasings()                       { return g_bulletCasings; }
    std::vector<ChristmasPresent>& GetChristmasPresents()               { return g_christmasPresents; }
    std::vector<ChristmasLights>& GetChristmasLights()                  { return g_christmasLights; }
    std::vector<ChristmasTree>& GetChristmasTrees()                     { return g_christmasTrees; }
    std::vector<ClippingCube>& GetClippingCubes()                       { return g_clippingCubes; }
    std::vector<Decal>& GetDecals()                                     { return g_decals; }
    std::vector<Door>& GetDoors()                                       { return g_doors; }
    std::vector<Drawers>& GetDrawers()                                  { return g_drawers; }
    std::vector<GameObject>& GetGameObjects()                           { return g_gameObjects; }
    std::vector<GenericBouncable>& GetGenericBouncables()               { return g_genericBouncables; }
    std::vector<GenericStatic>& GetGenericStatics()                     { return g_genericStatics; }
    std::vector<Plane>& GetPlanes()                                     { return g_planes; }
    std::vector<Light>& GetLights()                                     { return g_lights; };
    std::vector<Kangaroo>& GetKangaroos()                               { return g_kangaroos; }
    std::vector<MapInstance>& GetMapInstances()                         { return g_mapInstances; }
    std::vector<Mermaid>& GetMermaids()                                 { return g_mermaids; }
    std::vector<Piano>& GetPianos()                                     { return g_pianos; }
    std::vector<PickUp>& GetPickUps()                                   { return g_pickUps; }
    std::vector<PictureFrame>& GetPictureFrames()                       { return g_pictureFrames; }
    std::vector<SpawnPoint>& GetCampaignSpawnPoints()                   { return g_spawnCampaignPoints; }
    std::vector<SpawnPoint>& GetDeathmatchSpawnPoints()                 { return g_spawnDeathmatchPoints; }
    std::vector<Transform>& GetDoorAndWindowCubeTransforms()            { return g_doorAndWindowCubeTransforms; }
    std::vector<Toilet>& GetToilets()                                   { return g_toilets; }
    std::vector<Road>& GetRoads()                                       { return g_roads; }
    std::vector<Shark>& GetSharks()                                     { return g_sharks; }
    std::vector<Tree>& GetTrees()                                       { return g_trees; }
    std::vector<Wall>& GetWalls()                                       { return g_walls; }
    std::vector<VolumetricBloodSplatter>& GetVolumetricBloodSplatters() { return g_volumetricBloodSplatters; }
    std::vector<Window>& GetWindows()                                   { return g_windows; }

    std::vector<GPULight>& GetGPULightsLowRes()                 { return g_gpuLightsLowRes; }
    std::vector<GPULight>& GetGPULightsMidRes()                 { return g_gpuLightsMidRes; }
    std::vector<GPULight>& GetGPULightsHighRes()                { return g_gpuLightsHighRes; }
}