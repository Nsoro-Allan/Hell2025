#pragma once
#pragma warning(push, 0)
#include <physx/PxShape.h>
#include <physx/PxRigidDynamic.h>
#pragma warning(pop)

#include "HellTypes.h"
#include "Math/AABB.h"

using namespace physx;

struct RigidDynamic {
    void Update(float deltaTime);
    void ActivatePhsyics();
    void DeactivatePhysics();
    void SetPxRigidDynamic(PxRigidDynamic* rigidDynamic);
    void SetPxShape(PxShape* shape);
    void SetFilterData(PhysicsFilterData filterData);
    void MarkForRemoval();
    void UpdateMassAndInertia(float density);
    float GetVolume();

    bool HasActivePhysics()                 { return m_activePhysics; }
    bool IsMarkedForRemoval()               { return m_markedForRemoval; }
    PxRigidDynamic* GetPxRigidDynamic()     { return m_pxRigidDynamic; }
    PxShape* GetPxShape()                   { return m_pxShape; }
    const AABB& GetAABB()                   { return m_aabb; }
    const glm::vec3& GetCurrentPosition()   { return m_currentPosition; }
    const glm::vec3& GetPreviousPosition()  { return m_previousPosition; }
    const glm::mat4& GetWorldTransform()    { return m_worldTransform; }

private:
    AABB m_aabb;
    PxShape* m_pxShape = nullptr;
    PxRigidDynamic* m_pxRigidDynamic = nullptr;
    glm::vec3 m_currentPosition;
    glm::vec3 m_previousPosition;
    glm::mat4 m_worldTransform = glm::mat4(1.0f);
    float m_stationaryTime = 0;
    bool m_activePhysics = false;
    bool m_markedForRemoval = false;
};