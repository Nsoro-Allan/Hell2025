#include "RagdollInfo.h"
#include "HellLogging.h"
#include "Physics/Physics.h"
#include "Util.h"

void RagdollInfo::PrintMarkerInfo(const RagdollMarker& marker) {
    std::cout << "Name:                          " << marker.name << "\n";
    std::cout << "Bone path:                     " << marker.bonePath << "\n";
    std::cout << "Bone name:                     " << marker.boneName << "\n";
    std::cout << "Shape type:                    " << marker.shapeType << "\n";
    std::cout << "Mass:                          " << marker.mass << "\n";
    std::cout << "Contact stiffness:             " << marker.contactStiffness << "\n";
    std::cout << "Contact damping:               " << marker.contactDamping << "\n";
    std::cout << "Drive slerp:                   " << Util::BoolToString(marker.driveSlerp) << "\n";
    std::cout << "Drive spring type:             " << marker.driveSpringType << "\n";
    std::cout << "Linear stiffness:              " << marker.linearStiffness << "\n";
    std::cout << "Linear dampingRatio:           " << marker.linearDampingRatio << "\n";
    std::cout << "Angular stiffness:             " << marker.angularStiffness << "\n";
    std::cout << "Angular dampingRatio:          " << marker.angularDampingRatio << "\n";
    std::cout << "Input type:                    " << marker.inputType << "\n";
    std::cout << "Collision group:               " << marker.collisionGroup << "\n";
    std::cout << "Use root stiffness:            " << Util::BoolToString(marker.useRootStiffness) << "\n";
    std::cout << "Use linear angular stiffness:  " << Util::BoolToString(marker.useLinearAngularStiffness) << "\n";
    std::cout << "Limit range (twist,s1,s2):     " << marker.limitRange << "\n";
    std::cout << "Extents:                       " << marker.extents << "\n";
    std::cout << "Shape rotation:                " << marker.shapeRotation << "\n";
    std::cout << "Shape offset:                  " << marker.shapeOffset << "\n";
    std::cout << "Shape radius:                  " << marker.shapeRadius << "\n";
    std::cout << "Shape length:                  " << marker.shapeLength << "\n";
    std::cout << "Mesh vertex count:             " << static_cast<unsigned>(marker.convexMeshVertices.size()) << "\n";
    std::cout << "Mesh index count:              " << static_cast<unsigned>(marker.convexMeshIndices.size()) << "\n";
    std::cout << "linearDamping:                 " << marker.linearDamping << "\n";
    std::cout << "angularDamping:                " << marker.angularDamping << "\n";
    std::cout << "Is kinematic:                  " << Util::BoolToString(marker.isKinematic) << "\n";
    std::cout << "EnableCCD                      " << Util::BoolToString(marker.enableCCD) << "\n";
    std::cout << "Input matrix:\n" << marker.inputMatrix << "\n";
    std::cout << "Origin matrix:\n" << marker.originMatrix << "\n";
    std::cout << "Parent frame:\n" << marker.parentFrame << "\n";
    std::cout << "Child frame:\n" << marker.childFrame << "\n\n";
}

void RagdollInfo::PrintJointInfo(const RagdollJoint& joint) {
    std::cout << "Name:                          " << joint.name << "\n";
    std::cout << "Child name:                    " << joint.childName << "\n";
    std::cout << "Parent name:                   " << joint.parentName << "\n";
    std::cout << "Child (json id):               " << joint.childJsonId << "\n";
    std::cout << "Parent (json id):              " << joint.parentJsonId << "\n";
    std::cout << "Relative (json id):            " << joint.relativeJsonId << "\n";
    std::cout << "Limit linear:                  " << RdVector(joint.limitLinear) << "\n";
    std::cout << "Linear motion string:          " << joint.linearMotionString << "\n";
    std::cout << "Linear motion:                 " << joint.linearMotion << "\n";
    std::cout << "Limit range (twist,s1,s2):     " << joint.limitRange << "\n";
    std::cout << "Limit stiffness:               " << joint.limitStiffness << "\n";
    std::cout << "Limit dampingRatio:            " << joint.limitDampingRatio << "\n";
    std::cout << "Limit auto-orient:             " << Util::BoolToString(joint.limitAutoOrient) << "\n";
    std::cout << "Drive slerp:                   " << Util::BoolToString(joint.driveSlerp) << "\n";
    std::cout << "Drive spring type:             " << joint.driveSpringType << "\n";
    std::cout << "Drive linear stiffness:        " << joint.driveLinearStiffness << "\n";
    std::cout << "Drive linear damping:          " << joint.driveLinearDamping << "\n";
    std::cout << "Drive angular stiffness:       " << joint.driveAngularStiffness << "\n";
    std::cout << "Drive angular damping:         " << joint.driveAngularDamping << "\n";
    std::cout << "Drive linear amount:           " << joint.driveLinearAmount << "\n";
    std::cout << "Drive angular amt (twist):     " << joint.driveAngularAmountTwist << "\n";
    std::cout << "Drive angular amt (swing):     " << joint.driveAngularAmountSwing << "\n";
    std::cout << "Drive max linear force:        " << joint.driveMaxLinearForce << "\n";
    std::cout << "Drive max angular force:       " << joint.driveMaxAngularForce << "\n";
    std::cout << "Drive worldspace:              " << Util::BoolToString(joint.driveWorldspace) << "\n";
    std::cout << "Ignore mass:                   " << Util::BoolToString(joint.ignoreMass) << "\n";
    std::cout << "Parent frame:\n" << joint.parentFrame << "\n";
    std::cout << "Child frame:\n" << joint.childFrame << "\n";
    std::cout << "Drive target:\n" << joint.driveTarget << "\n";
    std::cout << "\n";
}

void RagdollInfo::PrintSolverInfo() {
    std::cout << "Ragdoll Solver\n";
    std::cout << "positionIterations: " << m_solver.positionIterations << "\n";
    std::cout << "gravity: " << m_solver.gravity << "\n";
    std::cout << "sceneScale: " << m_solver.sceneScale << "\n";
    std::cout << "linearLimitStiffness: " << m_solver.linearLimitStiffness << "\n";
    std::cout << "linearLimitDamping: " << m_solver.linearLimitDamping << "\n";
    std::cout << "angularLimitStiffness: " << m_solver.angularLimitStiffness << "\n";
    std::cout << "angularLimitDamping: " << m_solver.angularLimitDamping << "\n";
}

const RagdollMarker* RagdollInfo::GetMarkerByName(const RdString& name) const {
    for (const auto& m : m_markers)
        if (m.name == name) return &m;

    Logging::Error() << "RagdollV2::GetMarkerByName() failed to find " << name;
    return nullptr;
}