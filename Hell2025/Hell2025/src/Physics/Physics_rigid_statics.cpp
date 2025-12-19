#include "Physics.h"
#include "UniqueID.h"
#include "AssetManagement/AssetManager.h"
#include "Physics/Types/RigidStatic.h"
#include <unordered_map>
#include <vector>
#include "Util.h"

namespace Physics {

    std::unordered_map<uint64_t, RigidStatic> g_rigidStatics;

    uint64_t CreateRigidStaticBoxFromExtents(Transform transform, glm::vec3 boxExtents, PhysicsFilterData filterData, Transform localOffset) {
        PxPhysics* pxPhysics = Physics::GetPxPhysics();
        PxScene* pxScene = Physics::GetPxScene();
        PxMaterial* material = Physics::GetDefaultMaterial();

        float halfWidth = boxExtents.x * 0.5f;
        float halfHeight = boxExtents.y * 0.5f;
        float halfDepth = boxExtents.z * 0.5f;

        PxFilterData pxFilterData;
        pxFilterData.word0 = (PxU32)filterData.raycastGroup;
        pxFilterData.word1 = (PxU32)filterData.collisionGroup;
        pxFilterData.word2 = (PxU32)filterData.collidesWith;

        // Create shape
        PxShape* pxShape = pxPhysics->createShape(PxBoxGeometry(halfWidth, halfHeight, halfDepth), *material, true);
        pxShape->setQueryFilterData(pxFilterData);       // ray casts
        pxShape->setSimulationFilterData(pxFilterData);  // collisions
        pxShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
        pxShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);

        PxTransform localOffsetTransform = PxTransform(GlmMat4ToPxMat44(localOffset.to_mat4()));
        pxShape->setLocalPose(localOffsetTransform);

        // Create rigid dynamic
        PxQuat quat = Physics::GlmQuatToPxQuat(glm::quat(transform.rotation));
        PxTransform pxTransform = PxTransform(PxVec3(transform.position.x, transform.position.y, transform.position.z), quat);
        PxRigidStatic* pxRigidStatic = pxPhysics->createRigidStatic(pxTransform);
        pxRigidStatic->attachShape(*pxShape);
        pxScene->addActor(*pxRigidStatic);

        // Create DynamicBox
        uint64_t physicsID = UniqueID::GetNextPhysicsId();
        RigidStatic& rigidDynamic = g_rigidStatics[physicsID];

        // Update its pointers
        rigidDynamic.SetPxRigidStatic(pxRigidStatic);
        rigidDynamic.AddPxShape(pxShape);

        return physicsID;
    }


    uint64_t CreateRigidStaticFromCapsule(Transform transform, float radius, float halfHeight, PhysicsFilterData filterData, Transform localOffset) {
        PxPhysics* pxPhysics = Physics::GetPxPhysics();
        PxScene* pxScene = Physics::GetPxScene();
        PxMaterial* material = Physics::GetDefaultMaterial();

        PxFilterData pxFilterData;
        pxFilterData.word0 = (PxU32)filterData.raycastGroup;
        pxFilterData.word1 = (PxU32)filterData.collisionGroup;
        pxFilterData.word2 = (PxU32)filterData.collidesWith;

        PxCapsuleGeometry geo;
        geo.radius = radius;
        geo.halfHeight = halfHeight;

        // Create shape
        PxShape* pxShape = pxPhysics->createShape(geo, *material, true);
        pxShape->setQueryFilterData(pxFilterData);       // ray casts
        pxShape->setSimulationFilterData(pxFilterData);  // collisions
        pxShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
        pxShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);

        PxTransform localOffsetTransform = PxTransform(GlmMat4ToPxMat44(localOffset.to_mat4()));
        pxShape->setLocalPose(localOffsetTransform);

        // Create rigid dynamic
        PxQuat quat = Physics::GlmQuatToPxQuat(glm::quat(transform.rotation));
        PxTransform pxTransform = PxTransform(PxVec3(transform.position.x, transform.position.y, transform.position.z), quat);
        PxRigidStatic* pxRigidStatic = pxPhysics->createRigidStatic(pxTransform);
        pxRigidStatic->attachShape(*pxShape);
        pxScene->addActor(*pxRigidStatic);

        // Create DynamicBox
        uint64_t physicsID = UniqueID::GetNextPhysicsId();
        RigidStatic& rigidDynamic = g_rigidStatics[physicsID];

        // Update its pointers
        rigidDynamic.SetPxRigidStatic(pxRigidStatic);
        rigidDynamic.AddPxShape(pxShape);

        return physicsID;
    }

    uint64_t CreateRigidStaticConvexMeshFromVertices(Transform transform, const std::span<Vertex>& vertices, PhysicsFilterData filterData) {
       PxPhysics* pxPhysics = Physics::GetPxPhysics();
       PxScene* pxScene = Physics::GetPxScene();
       PxMaterial* material = Physics::GetDefaultMaterial();
  
       PxFilterData pxFilterData;
       pxFilterData.word0 = (PxU32)filterData.raycastGroup;
       pxFilterData.word1 = (PxU32)filterData.collisionGroup;
       pxFilterData.word2 = (PxU32)filterData.collidesWith;
  
       // Create convex shape
       std::vector<PxVec3> pxVertices;
       for (Vertex& vertex : vertices) {
           pxVertices.push_back(Physics::GlmVec3toPxVec3(vertex.position));
       }
  
       PxConvexMeshDesc convexDesc;
       convexDesc.points.count = pxVertices.size();
       convexDesc.points.stride = sizeof(PxVec3);
       convexDesc.points.data = pxVertices.data();
       convexDesc.flags = PxConvexFlag::eSHIFT_VERTICES | PxConvexFlag::eCOMPUTE_CONVEX;
       
       PxTolerancesScale scale;
       PxCookingParams params(scale);
  
       PxDefaultMemoryOutputStream buf;
       PxConvexMeshCookingResult::Enum result;
       if (!PxCookConvexMesh(params, convexDesc, buf, &result)) {
           std::cout << "some convex mesh shit failed\n";
           return 0;
       }
       PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
       PxConvexMesh* convexMesh = pxPhysics->createConvexMesh(input);
       PxConvexMeshGeometryFlags flags(~PxConvexMeshGeometryFlag::eTIGHT_BOUNDS);
       PxConvexMeshGeometry geometry(convexMesh, PxMeshScale(PxVec3(1.0f)), flags);
  
       PxShape* pxShape = pxPhysics->createShape(geometry, *material);
       pxShape->setQueryFilterData(pxFilterData);       // ray casts
       pxShape->setSimulationFilterData(pxFilterData);  // collisions
  
       // Create PxRigidStatic
       PxQuat quat = Physics::GlmQuatToPxQuat(glm::quat(transform.rotation));
       PxTransform pxTransform = PxTransform(PxVec3(transform.position.x, transform.position.y, transform.position.z), quat);
       PxRigidStatic* pxRigidStatic = pxPhysics->createRigidStatic(pxTransform);
       pxRigidStatic->attachShape(*pxShape);
       pxScene->addActor(*pxRigidStatic);
    
       // Create RigidStatic
       uint64_t physicsID = UniqueID::GetNextPhysicsId();
       RigidStatic& rigidStatic = g_rigidStatics[physicsID];
  
       // Update its pointers
       rigidStatic.SetPxRigidStatic(pxRigidStatic);
       rigidStatic.AddPxShape(pxShape);
  
       return physicsID;
   }

   uint64_t CreateRigidStaticConvexMeshFromModel(Transform transform, const std::string& modelName, PhysicsFilterData filterData) {
       PxPhysics* pxPhysics = Physics::GetPxPhysics();
       PxScene* pxScene = Physics::GetPxScene();
       PxMaterial* material = Physics::GetDefaultMaterial();

       PxFilterData pxFilterData;
       pxFilterData.word0 = (PxU32)filterData.raycastGroup;
       pxFilterData.word1 = (PxU32)filterData.collisionGroup;
       pxFilterData.word2 = (PxU32)filterData.collidesWith;

       Model* model = AssetManager::GetModelByName(modelName);
       if (!model) {
           std::cout << "Physics::CreateRigidStaticFromConvexMeshFromModel() failed: '" << modelName << "' was not found \"n";
           return 0;
       }

       // Create PxRigidStatic
       PxQuat quat = Physics::GlmQuatToPxQuat(glm::quat(transform.rotation));
       PxTransform pxTransform = PxTransform(PxVec3(transform.position.x, transform.position.y, transform.position.z), quat);
       PxRigidStatic* pxRigidStatic = pxPhysics->createRigidStatic(pxTransform);

       // Create RigidStatic
       uint64_t physicsID = UniqueID::GetNextPhysicsId();
       RigidStatic& rigidStatic = g_rigidStatics[physicsID];

       // Create convex shapes
       for (uint32_t meshIndex : model->GetMeshIndices()) {
           Mesh* mesh = AssetManager::GetMeshByIndex(meshIndex);
           std::span<Vertex> vertices = AssetManager::GetVerticesSpan(mesh->baseVertex, mesh->vertexCount);

           std::vector<PxVec3> pxVertices;
           for (Vertex& vertex : vertices) {
               pxVertices.push_back(Physics::GlmVec3toPxVec3(vertex.position));
           }

           PxConvexMeshDesc convexDesc;
           convexDesc.points.count = pxVertices.size();
           convexDesc.points.stride = sizeof(PxVec3);
           convexDesc.points.data = pxVertices.data();
           convexDesc.flags = PxConvexFlag::eSHIFT_VERTICES | PxConvexFlag::eCOMPUTE_CONVEX;

           PxTolerancesScale scale;
           PxCookingParams params(scale);

           PxDefaultMemoryOutputStream buf;
           PxConvexMeshCookingResult::Enum result;
           if (!PxCookConvexMesh(params, convexDesc, buf, &result)) {
               std::cout << "some convex mesh shit failed\n";
               return 0;
           }
           PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
           PxConvexMesh* convexMesh = pxPhysics->createConvexMesh(input);
           PxConvexMeshGeometryFlags flags(~PxConvexMeshGeometryFlag::eTIGHT_BOUNDS);
           PxConvexMeshGeometry geometry(convexMesh, PxMeshScale(PxVec3(1.0f)), flags);

           PxShape* pxShape = pxPhysics->createShape(geometry, *material);
           pxShape->setQueryFilterData(pxFilterData);       // ray casts
           pxShape->setSimulationFilterData(pxFilterData);  // collisions
           pxRigidStatic->attachShape(*pxShape);
           rigidStatic.AddPxShape(pxShape);
       }

       pxScene->addActor(*pxRigidStatic);

       // Update its pointers
       rigidStatic.SetPxRigidStatic(pxRigidStatic);

       std::cout << "APPARENTLY CRAETED CONVEX MESH: " << modelName << "\n";

       return physicsID;
   }



   uint64_t CreateRigidStaticTriangleMeshFromModel(Transform transform, const std::string& modelName, PhysicsFilterData filterData) {
       Model* model = AssetManager::GetModelByName(modelName);
       if (!model) {
           std::cout << "Physics::CreateRigidStaticTriangleMeshFromModel() failed: model name '" << modelName << "' not found\n";
           return 0;
       }

       std::vector<Vertex>& globalVertices = AssetManager::GetVertices();
       std::vector<uint32_t>& globalIndices = AssetManager::GetIndices();

       std::vector<Vertex> vertices;
       std::vector<uint32_t> indices;

       for (uint32_t meshIndex : model->GetMeshIndices()) {
           Mesh* mesh = AssetManager::GetMeshByIndex(meshIndex);
           if (!mesh) continue;

           for (int i = mesh->baseIndex; i < mesh->baseIndex + mesh->indexCount; i++) {
               uint32_t index = globalIndices[i];
               const Vertex& vertex = globalVertices[index + mesh->baseVertex];
               vertices.push_back(vertex);
               indices.push_back(static_cast<uint32_t>(vertices.size() - 1));
           }
       }

       return CreateRigidStaticTriangleMeshFromVertexData(transform, vertices, indices, filterData);
    }


   uint64_t CreateRigidStaticTriangleMeshFromVertexData(Transform transform, const std::span<Vertex>& vertices, const std::span<uint32_t>& indices, PhysicsFilterData filterData) {
       PxPhysics* pxPhysics = Physics::GetPxPhysics();
       PxScene* pxScene = Physics::GetPxScene();
       PxMaterial* material = Physics::GetDefaultMaterial();

       PxFilterData pxFilterData;
       pxFilterData.word0 = (PxU32)filterData.raycastGroup;
       pxFilterData.word1 = (PxU32)filterData.collisionGroup;
       pxFilterData.word2 = (PxU32)filterData.collidesWith;

       PxTriangleMeshDesc meshDesc;
       meshDesc.points.count = vertices.size();
       meshDesc.points.data = vertices.data();
       meshDesc.points.stride = sizeof(Vertex);
       meshDesc.triangles.count = indices.size() / 3;
       meshDesc.triangles.data = indices.data();
       meshDesc.triangles.stride = 3 * sizeof(PxU32);

       if (!meshDesc.isValid()) {
           std::cout << "PxTriangleMeshDesc is invalid!\n";
           std::cout << "Vertex count: " << meshDesc.points.count << "\n";
           std::cout << "Vertex stride: " << meshDesc.points.stride << "\n";
           std::cout << "Triangle count: " << meshDesc.triangles.count << "\n";
           std::cout << "Triangle stride: " << meshDesc.triangles.stride << "\n";
           return 0;
       }
       
       PxCookingParams params{ PxTolerancesScale() };
       params.midphaseDesc = PxMeshMidPhase::eBVH33;
       params.suppressTriangleMeshRemapTable = true;

       // Clear preprocessing flags using the proper PhysX API
       params.meshPreprocessParams.clear(PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH);
       params.meshPreprocessParams.clear(PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE);

       // Optimize BVH33 midphase for simulation performance
       params.midphaseDesc.mBVH33Desc.meshCookingHint = PxMeshCookingHint::eSIM_PERFORMANCE;
       params.midphaseDesc.mBVH33Desc.meshSizePerformanceTradeOff = 0.0f;

       // Validate the mesh cooking parameters
       PX_ASSERT(PxValidateTriangleMesh(params, meshDesc));

       // Create the triangle mesh
       PxTriangleMesh* pxTriangleMesh = PxCreateTriangleMesh(params, meshDesc, pxPhysics->getPhysicsInsertionCallback());

       // Create PxShape
       PxMeshGeometryFlags flags(~PxMeshGeometryFlag::eDOUBLE_SIDED);
       PxMeshScale meshScale = PxVec3(1.0f, 1.0f, 1.0f);
       PxTriangleMeshGeometry geometry(pxTriangleMesh, meshScale, flags);

       PxShapeFlags shapeFlags(PxShapeFlag::eSCENE_QUERY_SHAPE); // NOT eSIMULATION_SHAPE. PhysX does not allow for triangle mesh
       PxShape* pxShape = pxPhysics->createShape(geometry, *material, shapeFlags);
       pxShape->setQueryFilterData(pxFilterData);       // ray casts
       pxShape->setSimulationFilterData(pxFilterData);  // collisions

       // Create rigid static
       PxQuat quat = Physics::GlmQuatToPxQuat(glm::quat(transform.rotation));
       PxTransform pxTransform = PxTransform(PxVec3(transform.position.x, transform.position.y, transform.position.z), quat);
       PxRigidStatic* pxRigidStatic = pxPhysics->createRigidStatic(pxTransform);
       pxScene->addActor(*pxRigidStatic);
       pxRigidStatic->attachShape(*pxShape);

       // Create Rigid Static
       uint64_t physicsID = UniqueID::GetNextPhysicsId();
       RigidStatic& rigidStatic = g_rigidStatics[physicsID];
       rigidStatic.AddPxShape(pxShape);

       // Update its pointers
       rigidStatic.SetPxRigidStatic(pxRigidStatic);

       return physicsID;
   }

   void MarkRigidStaticForRemoval(uint64_t rigidStaticId) {
       if (RigidStaticExists(rigidStaticId)) {
           RigidStatic& rigidStatic = g_rigidStatics[rigidStaticId];
           rigidStatic.MarkForRemoval();
       }
   }

   void RemoveRigidStatic(uint64_t rigidStaticId) {
       if (RigidStaticExists(rigidStaticId)) {
           PxPhysics* pxPhysics = Physics::GetPxPhysics();
           PxScene* pxScene = Physics::GetPxScene();
           RigidStatic& rigidStatic = g_rigidStatics[rigidStaticId];
           PxRigidStatic* pxRigidStatic = rigidStatic.GetPxRigidStatic();

           // Remove rigid
           if (pxRigidStatic) {

               // Clean up its user data
               if (pxRigidStatic->userData) {
                   delete static_cast<PhysicsUserData*>(pxRigidStatic->userData);
                   pxRigidStatic->userData = nullptr;
               }
               // Remove it from PxScene
               if (pxRigidStatic->getScene() != nullptr) {
                   pxScene->removeActor(*pxRigidStatic);
               }
               // Release it
               pxRigidStatic->release();
               pxRigidStatic = nullptr;
           }

           // Remove the shapes
           std::vector<PxShape*>& pxShapes = rigidStatic.GetPxShapes();
           for (PxShape* pxShape : pxShapes) {
               if (pxShape) {
                   pxShape->release();
               }
           }

           // Remove from container
           g_rigidStatics.erase(rigidStaticId);
       }
   }

   int GetRigidStaticCount() {
       return g_rigidStatics.size();
   }

   void RemoveAnyRigidStaticMarkedForRemoval() {
       PxScene* pxScene = Physics::GetPxScene();

       for (auto it = g_rigidStatics.begin(); it != g_rigidStatics.end(); ) {
           RigidStatic& rigidStatic = it->second;
           if (rigidStatic.IsMarkedForRemoval()) {
               // Retrieve pointers
               PxRigidStatic* pxRigidStatic = rigidStatic.GetPxRigidStatic();

               // Remove the actor
               if (pxRigidStatic) {
                   if (pxRigidStatic->getScene() != nullptr) {
                       pxScene->removeActor(*pxRigidStatic);
                   }
               }

               // Release the shapes
               std::vector<PxShape*>& pxShapes = rigidStatic.GetPxShapes();
               for (PxShape* pxShape : pxShapes) {
                   if (pxShape) {
                       pxShape->release();
                   }
               }

               // Release the actor
               if (pxRigidStatic) {
                   pxRigidStatic->release();
               }

               // Remove from container
               it = g_rigidStatics.erase(it);
           }
           else {
               ++it;
           }
       }
   }

   glm::mat4 GetRigidStaticGlobalPose(uint64_t rigidStaticId) {
	   if (!RigidStaticExists(rigidStaticId)) return glm::mat4(1.0f);

	   RigidStatic& rigidStatic = g_rigidStatics[rigidStaticId];
	   PxRigidStatic* pxRigidStatic = rigidStatic.GetPxRigidStatic();
	   PxTransform pxTransform = pxRigidStatic->getGlobalPose();
	   return PxMat44ToGlmMat4(pxTransform);
   }

   void SetRigidStaticWorldTransform(uint64_t rigidStaticId, glm::mat4 worldMatrix) {
	   if (RigidStatic* rigidStatic = GetRigidStaitcById(rigidStaticId)) {
		   PxRigidStatic* pxRigidStatic = rigidStatic->GetPxRigidStatic();
		   PxMat44 pxWorldMatrix = Physics::GlmMat4ToPxMat44(worldMatrix);
		   pxRigidStatic->setGlobalPose(PxTransform(pxWorldMatrix));
	   }
   }

   void SetRigidStaticUserData(uint64_t m_physicsId, PhysicsUserData physicsUserData) {
       if (RigidStaticExists(m_physicsId)) {
           RigidStatic& rigidStatic = g_rigidStatics[m_physicsId];
           PxRigidStatic* pxRigidStatic = rigidStatic.GetPxRigidStatic();
           pxRigidStatic->userData = new PhysicsUserData(physicsUserData);
       }
   }

   bool RigidStaticExists(uint64_t rigidStaticId) {
       return (g_rigidStatics.find(rigidStaticId) != g_rigidStatics.end());
   }

   RigidStatic* GetRigidStaitcById(uint64_t rigidStaticId) {
       if (RigidStaticExists(rigidStaticId)) {
           return &g_rigidStatics[rigidStaticId];
       }
       else {
           return nullptr;
       }
   }
}