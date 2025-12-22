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
    void SetPxRigidDynamic(PxRigidDynamic* rigidDynamic);
    void SetPxShapes(const std::vector<PxShape*>& pxShapes);
    void SetFilterData(PhysicsFilterData filterData);
    void MarkForRemoval();
    void UpdateMassAndInertia(float density);
    float GetVolume();

    bool IsMarkedForRemoval()               { return m_markedForRemoval; }
    PxRigidDynamic* GetPxRigidDynamic()     { return m_pxRigidDynamic; }
    std::vector<PxShape*>& GetPxShapes()    { return m_pxShapes; }
    const AABB& GetAABB()                   { return m_aabb; }
    const glm::vec3& GetCurrentPosition()   { return m_currentPosition; }
    const glm::vec3& GetPreviousPosition()  { return m_previousPosition; }
    const glm::mat4& GetWorldTransform()    { return m_worldTransform; }
    size_t GetPxShapeCount() const          { return m_pxShapes.size(); }

private:
    AABB m_aabb; 
    std::vector<PxShape*> m_pxShapes;
    PxRigidDynamic* m_pxRigidDynamic = nullptr;
    glm::vec3 m_currentPosition;
    glm::vec3 m_previousPosition;
    glm::mat4 m_worldTransform = glm::mat4(1.0f);
    bool m_markedForRemoval = false;
};