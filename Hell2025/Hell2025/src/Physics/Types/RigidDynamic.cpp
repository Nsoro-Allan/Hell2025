#include "RigidDynamic.h"
#include "Physics/Physics.h"

void RigidDynamic::Update(float deltaTime) {
    if (!m_pxRigidDynamic) return;

    if (!m_pxRigidDynamic->getRigidBodyFlags().isSet(PxRigidBodyFlag::eKINEMATIC)) {
        // Update AABB
        PxBounds3 bounds = m_pxRigidDynamic->getWorldBounds();
        glm::vec3 aabbMin(bounds.minimum.x, bounds.minimum.y, bounds.minimum.z);
        glm::vec3 aabbMax(bounds.maximum.x, bounds.maximum.y, bounds.maximum.z);
        m_aabb = AABB(aabbMin, aabbMax);
    }

    m_worldTransform = Physics::PxMat44ToGlmMat4(m_pxRigidDynamic->getGlobalPose());
    m_currentPosition = m_worldTransform[3];
}

void RigidDynamic::MarkForRemoval() {
    m_markedForRemoval = true;
}

void RigidDynamic::SetPxRigidDynamic(PxRigidDynamic* rigidDynamic) {
    m_pxRigidDynamic = rigidDynamic; 
}

//void RigidDynamic::SetPxShape(PxShape* shape) {
//    m_pxShape = shape; 
//}

void RigidDynamic::SetPxShapes(const std::vector<PxShape*>& pxShapes) {
    m_pxShapes = pxShapes;
}

void RigidDynamic::SetFilterData(PhysicsFilterData filterData) {
    PxFilterData pxFilterData;
    pxFilterData.word0 = (PxU32)filterData.raycastGroup;
    pxFilterData.word1 = (PxU32)filterData.collisionGroup;
    pxFilterData.word2 = (PxU32)filterData.collidesWith;

    for (PxShape* pxShape : m_pxShapes) {
        pxShape->setQueryFilterData(pxFilterData);       // ray casts
        pxShape->setSimulationFilterData(pxFilterData);  // collisions
    }
}

float RigidDynamic::GetVolume() {
    float volume = 0.0f;
    for (PxShape* pxShape : m_pxShapes) {
        volume += Physics::ComputeShapeVolume(pxShape);
    }
    return volume;
}

void RigidDynamic::UpdateMassAndInertia(float density) {
    PxRigidBodyExt::updateMassAndInertia(*m_pxRigidDynamic, density);
}
