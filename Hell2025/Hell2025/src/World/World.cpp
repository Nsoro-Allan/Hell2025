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
#include "Managers/HouseManager.h"
#include "Managers/MapManager.h"
#include "Renderer/GlobalIllumination.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderDataManager.h"
#include "Physics/Physics.h"
#include "World/SectorManager.h"

#include "Pathfinding/AStarMap.h"

#include "Physics/Types/Ragdoll.h"

#include "SlotMap.h"

namespace World {
    std::vector<AnimatedGameObject> g_animatedGameObjects;
    std::vector<ScreenSpaceBloodDecal> g_screenSpaceBloodDecals;
    std::vector<Bullet> g_bullets;
    std::vector<BulletCasing> g_bulletCasings;
    std::vector<ChristmasLights> g_christmasLights;
    std::vector<ChristmasPresent> g_christmasPresents;
    std::vector<ChristmasTree> g_christmasTrees;
    std::vector<ClippingCube> g_clippingCubes;
    std::vector<Dobermann> g_dobermanns;
    std::vector<Decal> g_decals;
    std::vector<Fence> g_fences;
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
    std::vector<PowerPoleSet> g_powerPoleSets;
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

    Hell::SlotMap<Door> g_doors;
    Hell::SlotMap<Drawers> g_drawers;

    // std::unordered_map<uint64_t, HouseInstance> g_houseInstances; // unused???

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

        kangarooCreateInfo.position = glm::vec3(48, 32.6, 39);
        kangarooCreateInfo.rotation = glm::vec3(0, HELL_PI * -0.5f, 0);
        AddKangaroo(kangarooCreateInfo);

        kangarooCreateInfo.position = glm::vec3(48, 32.6, 36);
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
        LoadMapInstancesHeightMapData(mapInstanceCreateInfoSet);

        for (MapInstanceCreateInfo& mapInstanceCreateInfo : mapInstanceCreateInfoSet) {
            SpawnOffset spawnOffset;
            spawnOffset.translation.x = mapInstanceCreateInfo.spawnOffsetChunkX * HEIGHT_MAP_CHUNK_WORLD_SPACE_SIZE;
            spawnOffset.translation.z = mapInstanceCreateInfo.spawnOffsetChunkZ * HEIGHT_MAP_CHUNK_WORLD_SPACE_SIZE;

            // Load the objects
            LoadMapInstanceObjects(mapInstanceCreateInfo.mapName, spawnOffset);
            LoadMapInstanceHouses(mapInstanceCreateInfo.mapName, spawnOffset);
        }

        RecreateHouseMesh();

        // REMOVE ME BELOW TO MAP FILE
        PowerPoleSet& powerPoleSet = g_powerPoleSets.emplace_back();
        powerPoleSet.Init();

        Fence& fence = g_fences.emplace_back();
        fence.Init();

        GameObjectCreateInfo createInfo;
        //createInfo.position = glm::vec3(40.65f, 31.0f, 34.1f);
        //createInfo.modelName = "Drawers2";

        //AddGameObject(createInfo);
        //g_gameObjects[0].SetMeshMaterial("Frame", "T_Main_01a");
        //g_gameObjects[0].SetMeshMaterial("Drawers", "T_Drawers_01a");
        //g_gameObjects[0].SetMeshMaterial("Handles", "T_Handles_01a");
        //g_gameObjects[0].SetMeshMaterial("Key", "T_SmallKey_01a");

        //
        //createInfo2.position = glm::vec3(37.25f, 31.0f, 35.5f);
        //createInfo2.scale = glm::vec3(1.0f);
        //createInfo2.modelName = "DobermannTest";
        //AddGameObject(createInfo2);
        //g_gameObjects[1].SetMeshMaterial("Dobermann", "DobermannMouthBlood");
        //g_gameObjects[1].SetMeshMaterial("Iris", "DobermannIris");
        //g_gameObjects[1].SetMeshMaterial("Tongue", "DobermannMouthBlood");
        //g_gameObjects[1].SetMeshMaterial("Jaw", "DobermannMouthBlood");

        createInfo.position = glm::vec3(32.45f, 30.52f, 10.22f);
        createInfo.rotation.y = -HELL_PI * 0.5f;
        createInfo.scale = glm::vec3(1.0f);
        createInfo.modelName = "Reflector";
        AddGameObject(createInfo);
        g_gameObjects[0].SetMeshMaterial("ReflectorPole", "Fence");
        g_gameObjects[0].SetMeshMaterial("ReflectorRed", "Red");

        DobermannCreateInfo dobermannCreateInfo;
        //dobermannCreateInfo.position = glm::vec3(41.0f, 31.0f, 35.0f);
        //AddDobermann(dobermannCreateInfo);

        dobermannCreateInfo.position = glm::vec3(37.25f, 31.0f, 35.5f);
        AddDobermann(dobermannCreateInfo);

        // Add all this to the map editor tomorrow
        // Add all this to the map editor tomorrow
        // Add all this to the map editor tomorrow

        // Hack in a Christmas tree
        //ChristmasTreeCreateInfo christmasTreeCreateInfo;
        //christmasTreeCreateInfo.position = glm::vec3(8.13f, 0.15f, 1.2f);
        //christmasTreeCreateInfo.rotation.y = Util::RandomFloat(0, HELL_PI);
        //AddChristmasTree(christmasTreeCreateInfo, spawnOffset);
        //
        //christmasTreeCreateInfo.position = glm::vec3(0.78f, 0.15f, 2.25f);
        //christmasTreeCreateInfo.rotation.y = Util::RandomFloat(0, HELL_PI);
        //AddChristmasTree(christmasTreeCreateInfo, spawnOffset);
        //
        //
        //ChristmasLightsCreateInfo christmasLightsCreateInfo;
        //christmasLightsCreateInfo.start = glm::vec3(4.4, 2.13241, 0.861675);
        //christmasLightsCreateInfo.end = glm::vec3(2.99485, 2.16198, 2.9);
        //christmasLightsCreateInfo.spiral = false;
        //christmasLightsCreateInfo.sag = 0.6f;
        //World::AddChristmasLights(christmasLightsCreateInfo, spawnOffset);
        //
        //
        //glm::vec3 couchSpawnPosition = glm::vec3(0.02f, 0.0f, -1.5f);
        //glm::vec3 couchSpawnRotation = glm::vec3(0.0f, HELL_PI * 0.5f, 0.0f);
        //
        //GenericStaticCreateInfo genericStaticCreateInfo;
        //genericStaticCreateInfo.position = couchSpawnPosition;
        //genericStaticCreateInfo.rotation = couchSpawnRotation;
        //genericStaticCreateInfo.type = GenericStaticType::COUCH;
        //World::AddGenericStatic(genericStaticCreateInfo, spawnOffset);
        //
        //
        //GenericBouncableCreateInfo genericBouncableCreateInfo;
        //genericBouncableCreateInfo.position = couchSpawnPosition;
        //genericBouncableCreateInfo.rotation = couchSpawnRotation;
        //genericBouncableCreateInfo.rotation.y = HELL_PI * 0.5f;
        //genericBouncableCreateInfo.type = GenericBouncableType::COUCH_CUSHION_0;
        //World::AddGenericBouncable(genericBouncableCreateInfo, spawnOffset);
        //
        //genericBouncableCreateInfo.position = couchSpawnPosition;
        //genericBouncableCreateInfo.rotation = couchSpawnRotation;
        //genericBouncableCreateInfo.rotation.y = HELL_PI * 0.5f;
        //genericBouncableCreateInfo.type = GenericBouncableType::COUCH_CUSHION_0;
        //World::AddGenericBouncable(genericBouncableCreateInfo, spawnOffset);
        //
        //genericBouncableCreateInfo.position = couchSpawnPosition;
        //genericBouncableCreateInfo.rotation = couchSpawnRotation;
        //genericBouncableCreateInfo.rotation.y = HELL_PI * 0.5f;
        //genericBouncableCreateInfo.type = GenericBouncableType::COUCH_CUSHION_1;
        //World::AddGenericBouncable(genericBouncableCreateInfo, spawnOffset);
        //
        //genericBouncableCreateInfo.position = couchSpawnPosition;
        //genericBouncableCreateInfo.rotation = couchSpawnRotation;
        //genericBouncableCreateInfo.rotation.y = HELL_PI * 0.5f;
        //genericBouncableCreateInfo.type = GenericBouncableType::COUCH_CUSHION_2;
        //World::AddGenericBouncable(genericBouncableCreateInfo, spawnOffset);
        //
        //genericBouncableCreateInfo.position = couchSpawnPosition;
        //genericBouncableCreateInfo.rotation = couchSpawnRotation;
        //genericBouncableCreateInfo.rotation.y = HELL_PI * 0.5f;
        //genericBouncableCreateInfo.type = GenericBouncableType::COUCH_CUSHION_3;
        //World::AddGenericBouncable(genericBouncableCreateInfo, spawnOffset);
        //
        //ToiletCreateInfo toiletCreateInfo;
        //toiletCreateInfo.position = glm::vec3(4.40f, 0.0f, -0.7f);
        //toiletCreateInfo.rotation.y = HELL_PI * 0.5f;
        //World::AddToilet(toiletCreateInfo, spawnOffset);
        //


    }

    void LoadMapInstancesHeightMapData(std::vector<MapInstanceCreateInfo> mapInstanceCreateInfoSet) {
        g_mapInstances.clear();
        g_worldMapChunkCountX = 0;
        g_worldMapChunkCountZ = 0;

        // Load height map data from all map instances
        for (MapInstanceCreateInfo& mapInstanceCreateInfo : mapInstanceCreateInfoSet) {
            int32_t mapIndex = MapManager::GetMapIndexByName(mapInstanceCreateInfo.mapName);
            Map* map = MapManager::GetMapByName(mapInstanceCreateInfo.mapName);
            if (!map) {
                Logging::Error() << "World::LoadMapInstancesHeightMapData() failed coz '" << mapInstanceCreateInfo.mapName << "' was not found";
                return;
            }

            MapInstance& mapInstance = g_mapInstances.emplace_back();
            mapInstance.m_mapIndex = mapIndex;
            mapInstance.spawnOffsetChunkX = mapInstanceCreateInfo.spawnOffsetChunkX;
            mapInstance.spawnOffsetChunkZ = mapInstanceCreateInfo.spawnOffsetChunkZ;

            uint32_t reachX = mapInstance.spawnOffsetChunkX + map->GetChunkCountX();
            uint32_t reachZ = mapInstance.spawnOffsetChunkZ + map->GetChunkCountZ();

            g_worldMapChunkCountX = std::max(g_worldMapChunkCountX, reachX);
            g_worldMapChunkCountZ = std::max(g_worldMapChunkCountZ, reachZ);
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
    }

    void LoadSingleHouse(const std::string& houseName) {
        ResetWorld();
        LoadHouseInstance(houseName, SpawnOffset());
        RecreateHouseMesh();
    }

    void LoadHouseInstance(const std::string& houseName, SpawnOffset spawnOffset) {
        House* house = HouseManager::GetHouseByName(houseName);
        if (!house) {
            Logging::Error() << "World::LoadHouseInstance() failed because " << houseName << " was not found";
            return;
        }

        CreateInfoCollection& createInfoCollection = house->GetCreateInfoCollection();
        AddCreateInfoCollection(createInfoCollection, spawnOffset);

        Logging::Debug() << "World::LoadHouseInstance(): " << houseName << " at " << spawnOffset.translation;
    }

    void AddCreateInfoCollection(CreateInfoCollection& createInfoCollection, SpawnOffset spawnOffset) {
        for (DoorCreateInfo& createInfo : createInfoCollection.doors)                   AddDoor(createInfo, spawnOffset);
        for (DrawersCreateInfo& createInfo : createInfoCollection.drawers)              AddDrawers(createInfo, spawnOffset);
        for (LightCreateInfo& createInfo : createInfoCollection.lights)                 AddLight(createInfo, spawnOffset);
        for (PianoCreateInfo& createInfo : createInfoCollection.pianos)                 AddPiano(createInfo, spawnOffset);
        for (PickUpCreateInfo& createInfo : createInfoCollection.pickUps)               AddPickUp(createInfo, spawnOffset);
        for (PictureFrameCreateInfo& createInfo : createInfoCollection.pictureFrames)   AddPictureFrame(createInfo, spawnOffset);
        for (PlaneCreateInfo& createInfo : createInfoCollection.planes)                 AddHousePlane(createInfo, spawnOffset);
        for (TreeCreateInfo& createInfo : createInfoCollection.trees)                   AddTree(createInfo, spawnOffset);
        for (WallCreateInfo& createInfo : createInfoCollection.walls)                   AddWall(createInfo, spawnOffset);
        for (WindowCreateInfo& createInfo : createInfoCollection.windows)               AddWindow(createInfo, spawnOffset);
    }

    CreateInfoCollection GetCreateInfoCollection() {
        CreateInfoCollection createInfoCollection;

        for (Door& door : World::GetDoors())                        createInfoCollection.doors.push_back(door.GetCreateInfo());
        for (Drawers& drawers : World::GetDrawers())                createInfoCollection.drawers.push_back(drawers.GetCreateInfo());
        for (Light& light : World::GetLights())                     createInfoCollection.lights.push_back(light.GetCreateInfo());
        for (Piano& piano : World::GetPianos())                     createInfoCollection.pianos.push_back(piano.GetCreateInfo());
        for (PickUp& pickUp : World::GetPickUps())                  createInfoCollection.pickUps.push_back(pickUp.GetCreateInfo());
        for (Plane& plane : World::GetPlanes())                     createInfoCollection.planes.push_back(plane.GetCreateInfo());
        for (PictureFrame& pictureFrame : World::GetPictureFrames()) createInfoCollection.pictureFrames.push_back(pictureFrame.GetCreateInfo());
        for (Tree& tree : World::GetTrees())                        createInfoCollection.trees.push_back(tree.GetCreateInfo());
        for (Wall& wall : World::GetWalls())                        createInfoCollection.walls.push_back(wall.GetCreateInfo());
        for (Window& window : World::GetWindows())                  createInfoCollection.windows.push_back(window.GetCreateInfo());

        return createInfoCollection;
    }

    void LoadMapInstanceObjects(const std::string& mapName, SpawnOffset spawnOffset) {
        Map* map = MapManager::GetMapByName(mapName);
        if (!map) {
            Logging::Error() << "World::LoadMapInstanceObjects() failed coz '" << mapName << "' was not found";
            return;
        }

        // Add EVERYTHING: doors, walls, draws, toilets, pianos, etc...
        AddCreateInfoCollection(map->GetCreateInfoCollection(), spawnOffset);

        // Load campaign spawn points
        for (SpawnPoint& spawnPoint : map->GetAdditionalMapData().playerCampaignSpawns) {
            SpawnPoint& addedSpawnPoint = g_spawnCampaignPoints.emplace_back(spawnPoint);
            addedSpawnPoint.Init();
        }

        // Load deathmatch spawn points
        for (SpawnPoint& spawnPoint : map->GetAdditionalMapData().playerDeathmatchSpawns) {
            SpawnPoint& addedSpawnPoint = g_spawnDeathmatchPoints.emplace_back(spawnPoint);
            addedSpawnPoint.Init();
        }
    }

    void LoadMapInstanceHouses(const std::string& mapName, SpawnOffset spawnOffset) {
        Map* map = MapManager::GetMapByName(mapName);
        if (!map) {
            Logging::Error() << "World::LoadMapInstanceHouses() failed coz '" << mapName << "' was not found";
            return;
        }

        for (HouseLocation& houseLocation : map->GetAdditionalMapData().houseLocations) {
            SpawnOffset houseSpawnOffset = spawnOffset;
            houseSpawnOffset.translation += houseLocation.position;
            houseSpawnOffset.yRotation += houseLocation.rotation;

            LoadHouseInstance("TestHouse", houseSpawnOffset);
        }
    }

    void NewRun() {
        ResetWorld();

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

    Door* GetDoorByObjectId(uint64_t objectId) {
        return g_doors.get(objectId);
    }

    Drawers* GetDrawersByObjectId(uint64_t objectId) {
        return g_drawers.get(objectId);
    }






    //Door* GetDoorByDoorFrameObjectId(uint64_t objectID) {
    //    for (int i = 0; i < g_doors.size(); i++) {
    //        Door& door = g_doors[i];
    //        if (door.GetFrameObjectId() == objectID) {
    //            return &g_doors[i];
    //        }
    //    }
    //    return nullptr;
    //}







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

    AnimatedGameObject* GetAnimatedGameObjectByObjectId(uint64_t objectID) {
        for (AnimatedGameObject& animatedGameObject : g_animatedGameObjects) {
            if (animatedGameObject.GetObjectId() == objectID) {
                return &animatedGameObject;
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
        Drawers* drawers = World::GetDrawersByObjectId(objectId);
        if (drawers) {
            drawers->SetPosition(position);
        }

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

    void SetObjectRotation(uint64_t objectId, glm::vec3 rotation) {
        Drawers* drawers = World::GetDrawersByObjectId(objectId);
        if (drawers) {
            drawers->SetRotation(rotation);
        }
    }

    glm::vec3 GetGizmoOffest(uint64_t objectId) {
        Drawers* drawers = World::GetDrawersByObjectId(objectId);
        if (drawers) {
            return drawers->GetGizmoOffset();
        }
        return glm::vec3(0.0f);
    }

    bool RemoveObject(uint64_t objectId) {
        if (g_doors.contains(objectId)) {
            g_doors.get(objectId)->CleanUp();
            g_doors.erase(objectId);
            return true;
        }
        if (g_drawers.contains(objectId)) {
            g_drawers.get(objectId)->CleanUp();
            g_drawers.erase(objectId);
            return true;
        }



        for (int i = 0; i < g_pianos.size(); i++) {
            if (g_pianos[i].GetObjectId() == objectId) {
                g_pianos[i].CleanUp();
                g_pianos.erase(g_pianos.begin() + i);
                return true;
            }
        }
        for (int i = 0; i < g_planes.size(); i++) {
            if (g_planes[i].GetObjectId() == objectId) {
                g_planes[i].CleanUp();
                g_planes.erase(g_planes.begin() + i);
                return true;
            }
        }
        for (int i = 0; i < g_pickUps.size(); i++) {
            if (g_pickUps[i].GetObjectId() == objectId) {
                g_pickUps[i].CleanUp();
                g_pickUps.erase(g_pickUps.begin() + i);
                return true;
            }
        }
        for (int i = 0; i < g_walls.size(); i++) {
            if (g_walls[i].GetObjectId() == objectId) {
                g_walls[i].CleanUp();
                g_walls.erase(g_walls.begin() + i);
                return true;
            }
        }
        for (int i = 0; i < g_windows.size(); i++) {
            if (g_windows[i].GetObjectId() == objectId) {
                g_windows[i].CleanUp();
                g_windows.erase(g_windows.begin() + i);
                return true;
            }
        }

        for (int i = 0; i < g_animatedGameObjects.size(); i++) {
            if (g_animatedGameObjects[i].GetObjectId() == objectId) {
                g_animatedGameObjects[i].CleanUp();
                g_animatedGameObjects.erase(g_animatedGameObjects.begin() + i);
                return true;
            }
        }

        for (int i = 0; i < g_trees.size(); i++) {
            if (g_trees[i].GetObjectId() == objectId) {
                Logging::Debug() << "Deleted " << g_trees[i].GetEditorName();
                g_trees[i].CleanUp();
                g_trees.erase(g_trees.begin() + i);
                return true;
            }
        }

        Logging::Error() << "World::RemoveObject() Failed to remove object " << objectId << ", check you have implemented this type!\n";
        return false;
    }

    void ResetWorld() {
        std::cout << "Reset world()\n";

        // Clear heightmap data
        g_heightMapChunks.clear();
        g_validChunks.clear();
        g_mapInstances.clear();

        //RemoveAllHouseBvhs();

        // Cleanup all objects
        ClearAllObjects();

        MermaidCreateInfo mermaidCreateInfo;
        mermaidCreateInfo.position = glm::vec3(29.0f, 29.5f, 52.5f);
        mermaidCreateInfo.rotation.y = 0.25f;
        AddMermaid(mermaidCreateInfo);

        // remove meeeeeeeeeeeeeeeee
        // remove meeeeeeeeeeeeeeeee


        // remove meeeeeeeeeeeeeeeee
        // remove meeeeeeeeeeeeeeeee

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
        ResetWeatherboardMeshBuffer();

        for (BulletCasing& bulletCasing : g_bulletCasings)              bulletCasing.CleanUp();
        for (ChristmasLights& christmasLights : g_christmasLights)      christmasLights.CleanUp();
        for (ChristmasPresent& christmasPresent : g_christmasPresents)  christmasPresent.CleanUp();
        for (ChristmasTree& christmasTree : g_christmasTrees)           christmasTree.CleanUp();
        for (Door& door : g_doors)                                      door.CleanUp();
        for (Drawers& drawer : g_drawers)                               drawer.CleanUp();
        for (Fence& fence : g_fences)                                   fence.CleanUp();
        for (GameObject& gameObject : g_gameObjects)                    gameObject.CleanUp();
        //for (GenericBouncable& genericBouncable : g_genericBouncables) genericStatic.CleanUp();
        for (GenericStatic& genericStatic : g_genericStatics)           genericStatic.CleanUp();
        //for (Kangaroo& kangaroo : g_kangaroos)                        kangaroo.CleanUp();
        for (Mermaid& mermaid : g_mermaids)                             mermaid.CleanUp();
        for (Plane& housePlane : g_planes)                              housePlane.CleanUp();
        for (Piano& piano : g_pianos)                                   piano.CleanUp();
        for (PickUp& pickUp : g_pickUps)                                pickUp.CleanUp();
        for (PowerPoleSet& powerPoleSet: g_powerPoleSets)               powerPoleSet.CleanUp();
        for (Shark& shark : g_sharks)                                   shark.CleanUp();
        for (SpawnPoint& spawnPoint : g_spawnCampaignPoints)            spawnPoint.CleanUp();
        for (SpawnPoint& spawnPoint : g_spawnDeathmatchPoints)          spawnPoint.CleanUp();
        for (Tree& tree : g_trees)                                      tree.CleanUp();
        for (Wall& wall : g_walls)                                      wall.CleanUp();
        for (Window& window : g_windows)                                window.CleanUp();

        //for (auto& [id, drawers] : g_drawers) drawers.CleanUp();
        
        // Clear all containers
        g_bulletCasings.clear();
        g_screenSpaceBloodDecals.clear();
        g_christmasLights.clear();
        g_christmasPresents.clear();
        g_christmasTrees.clear();
        g_decals.clear();
        g_doors.clear();
        g_drawers.clear();
        g_fences.clear();
        g_gameObjects.clear();
        g_genericStatics.clear();
        //g_kangaroos.clear();
        g_lights.clear();
        g_mermaids.clear();
        g_pianos.clear();
        g_pickUps.clear();
        g_planes.clear();
        g_pictureFrames.clear();
        g_powerPoleSets.clear();
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

    void AddDoor(DoorCreateInfo createInfo, SpawnOffset spawnOffset) {
        const uint64_t id = UniqueID::GetNext(ObjectType::DOOR);
        g_doors.emplace_with_id(id, id, createInfo, spawnOffset);
    }

    void AddDrawers(DrawersCreateInfo createInfo, SpawnOffset spawnOffset) {
        const uint64_t id = UniqueID::GetNext(ObjectType::DRAWERS);
        g_drawers.emplace_with_id(id, id, createInfo, spawnOffset);
    }




    void AddBullet(BulletCreateInfo createInfo) {
        g_bullets.push_back(Bullet(createInfo));
    }

    void AddBulletCasing(BulletCasingCreateInfo createInfo, SpawnOffset spawnOffset) {
        createInfo.position += spawnOffset.translation;
        g_bulletCasings.push_back(BulletCasing(createInfo));
    }

    void AddDecal(const DecalCreateInfo& createInfo) {
        Decal& decal = g_decals.emplace_back();
        decal.Init(createInfo);
    }

    void AddDobermann(DobermannCreateInfo& createInfo) {
        Dobermann& dobermann = g_dobermanns.emplace_back();
        dobermann.Init(createInfo);
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

    void PrintObjectCounts() {
        Logging::Debug()
            << "Doors:          " << g_doors.size() << "\n"
            << "Lights:         " << g_lights.size() << "\n"
            << "Pickups:        " << g_pickUps.size() << "\n"
            << "Pianos:         " << g_pianos.size() << "\n"
            << "Picture Frames: " << g_pictureFrames.size() << "\n"
            << "Planes:         " << g_planes.size() << "\n"
            << "Trees:          " << g_trees.size() << "\n"
            << "Walls:          " << g_walls.size() << "\n"
            << "Windows:        " << g_windows.size() << "\n"
            << "";

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
    Hell::SlotMap<Door>& GetDoors()                                     { return g_doors; }
    std::vector<Dobermann>& GetDobermanns()                             { return g_dobermanns; }
    Hell::SlotMap<Drawers>& GetDrawers()                                { return g_drawers; }
    std::vector<Fence>& GetFences()                                     { return g_fences; }
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
    std::vector<PowerPoleSet>& GetPowerPoleSets()                       { return g_powerPoleSets; }
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