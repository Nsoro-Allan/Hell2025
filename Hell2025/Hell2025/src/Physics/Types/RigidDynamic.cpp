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

        // Get velocity instead of relying on position delta
        PxVec3 linVel = m_pxRigidDynamic->getLinearVelocity();
        PxVec3 angVel = m_pxRigidDynamic->getAngularVelocity();
        float velocitySquared = linVel.magnitudeSquared() + angVel.magnitudeSquared();

        const float velocityThresholdSquared = 0.01f; // Adjust this threshold
        const float timeThreshold = 3.0f;             // Seconds to be considered "stationary"

        if (m_pxRigidDynamic->isSleeping() || velocitySquared < velocityThresholdSquared) {
            m_stationaryTime += deltaTime; // Accumulate stationary time
        }
        else {
            m_stationaryTime = 0.0f; // Reset if movement detected
        }

        // Convert to kinematic if stationary for enough time
        if (m_stationaryTime >= timeThreshold) {
            m_pxRigidDynamic->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
        }
    }

    m_worldTransform = Physics::PxMat44ToGlmMat4(m_pxRigidDynamic->getGlobalPose());
}

void RigidDynamic::ActivatePhsyics() {
    if (!m_activePhysics) {
        PxScene* pxScene = Physics::GetPxScene();
        pxScene->addActor(*m_pxRigidDynamic);
        m_pxShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
        m_pxShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
    }
    m_activePhysics = true;
}

void RigidDynamic::DeactivatePhysics() {
    if (m_activePhysics) {
        PxScene* pxScene = Physics::GetPxScene();
        m_pxShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
        m_pxShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, false);
        pxScene->removeActor(*m_pxRigidDynamic);
    }
    m_activePhysics = false;
}

void RigidDynamic::MarkForRemoval() {
    m_markedForRemoval = true;
}

void RigidDynamic::SetPxRigidDynamic(PxRigidDynamic* rigidDynamic) {
    m_pxRigidDynamic = rigidDynamic; 
}

void RigidDynamic::SetPxShape(PxShape* shape) {
    m_pxShape = shape; 
}

void RigidDynamic::SetFilterData(PhysicsFilterData filterData) {
    PxFilterData pxFilterData;
    pxFilterData.word0 = (PxU32)filterData.raycastGroup;
    pxFilterData.word1 = (PxU32)filterData.collisionGroup;
    pxFilterData.word2 = (PxU32)filterData.collidesWith;
    m_pxShape->setQueryFilterData(pxFilterData);       // ray casts
    m_pxShape->setSimulationFilterData(pxFilterData);  // collisions
}

float RigidDynamic::GetVolume() {
    return Physics::ComputeShapeVolume(m_pxShape);
}

void RigidDynamic::UpdateMassAndInertia(float density) {
    PxRigidBodyExt::updateMassAndInertia(*m_pxRigidDynamic, density);
}
