#include "Physics.h"

PxQueryHitType::Enum RaycastFilterCallback::preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags) {
    const PxFilterData sf = shape->getQueryFilterData();

    // Ignore explicit actors
    for (const PxRigidActor* pxRigidActor : m_ignoredActors) {
        if (actor == pxRigidActor) {
            return PxQueryHitType::eNONE;
        }
    }

    // Ignore raycast-disabled shapes (no overlapping bits with the query)
    if ((sf.word0 & filterData.word0) == 0) {
        return PxQueryHitType::eNONE;
    }

    return PxQueryHitType::eBLOCK;
}

PxQueryHitType::Enum RaycastFilterCallback::postFilter(const PxFilterData& filterData, const PxQueryHit& hit, const PxShape* shape, const PxRigidActor* actor) {
    return PxQueryHitType::eBLOCK;
}


void RaycastFilterCallback::AddIgnoredActor(PxRigidDynamic* pxRigidDynamic) {
    m_ignoredActors.push_back(pxRigidDynamic);
}

void RaycastFilterCallback::AddIgnoredActors(std::vector<PxRigidDynamic*> pxRigidDynamics) {
    m_ignoredActors.reserve(m_ignoredActors.size() + pxRigidDynamics.size());
    for (PxRigidDynamic* pxRigidDynamic : pxRigidDynamics) {
        if (pxRigidDynamic) m_ignoredActors.push_back(pxRigidDynamic);
    }
}

PxQueryHitType::Enum RaycastHeightFieldFilterCallback::preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags) {
    const PxGeometryHolder geomHolder = shape->getGeometry();
    if (geomHolder.getType() != PxGeometryType::eHEIGHTFIELD) {
        return PxQueryHitType::eNONE;
    }
    return PxQueryHitType::eBLOCK;
}

PxQueryHitType::Enum RaycastHeightFieldFilterCallback::postFilter(const PxFilterData& filterData, const PxQueryHit& hit, const PxShape* shape, const PxRigidActor* actor) {
    return PxQueryHitType::eBLOCK;
}

// Maybe find a better way to do this, coz it so similar to the above
// Maybe find a better way to do this, coz it so similar to the above
// Maybe find a better way to do this, coz it so similar to the above
// Maybe find a better way to do this, coz it so similar to the above

PxQueryHitType::Enum RaycastStaticEnviromentFilterCallback::preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags) {
    const PxGeometryHolder geomHolder = shape->getGeometry();
    if (geomHolder.getType() != PxGeometryType::eHEIGHTFIELD && geomHolder.getType() != PxGeometryType::eTRIANGLEMESH) {
        return PxQueryHitType::eNONE;
    }
    return PxQueryHitType::eBLOCK;
}

PxQueryHitType::Enum RaycastStaticEnviromentFilterCallback::postFilter(const PxFilterData& filterData, const PxQueryHit& hit, const PxShape* shape, const PxRigidActor* actor) {
    return PxQueryHitType::eBLOCK;
}

