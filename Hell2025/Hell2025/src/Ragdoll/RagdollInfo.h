#pragma once
#include "Types.h"
#include <iostream>
#include "Util.h"

struct RdGeometryDescriptionComponent {
    float length{ 1.0f };
    float radius{ 1.0f };
    float radiusEnd{ 1.0f };
    RdGeometryType type{ RdGeometryType::kSphere };
    RdVector extents{ 1.0, 1.0, 1.0 };
    RdVector offset{ 0.0, 0.0, 0.0 };
    RdEulerRotation rotation{ 0.0, 0.0, 0.0 };
    bool capsuleLengthAlongY = true;
    enum RdConvexDecomposition convexDecomposition = RdConvexDecomposition::Off;
};

struct RdScaleComponent {
    RdVector value{ 1.0, 1.0, 1.0 };
    RdVector absolute{ 1.0, 1.0, 1.0 };
};

struct RagdollMarker {
    // Transforms
    RdString name = UNDEFINED_STRING;
    RdMatrix inputMatrix{ RdIdentityInit };
    RdMatrix originMatrix{ RdIdentityInit };
    RdMatrix parentFrame{ RdIdentityInit };
    RdMatrix childFrame{ RdIdentityInit };

    RdGeometryDescriptionComponent geometryDescriptionComponent;
    RdScaleComponent scaleComponent;

    RdPoints convexMeshVertices{};
    RdUints convexMeshIndices{};

    // Limits (angular)
    RdEulerRotation limitRange{};

    // Contact/rigid properties
    RdScalar contactStiffness{ 0.0 };
    RdScalar contactDamping{ 0.0 };
    RdScalar mass{ 0.0 };

    // Drive UI bits mirrored on the marker
    RdBoolean driveSlerp{ false };
    RdEnum driveSpringType{ 0 }; // 0=force, 1=accel

    // Stiffness/damping UI
    RdScalar linearStiffness{ 0.0 };
    RdScalar linearDampingRatio{ 0.0 };
    RdScalar angularStiffness{ 0.0 };
    RdScalar angularDampingRatio{ 0.0 };

    // Misc UI
    RdEnum inputType{ 0 }; // RdBehaviour backing value
    RdInteger collisionGroup{ 0 };
    RdBoolean useRootStiffness{ false };
    RdBoolean useLinearAngularStiffness{ false };
    RdColor color;

    RdBoolean isKinematic = false;
    RdBoolean enableCCD = false;
    RdScalar linearDamping = 0.0f;
    RdScalar angularDamping = 0.05f;
    RdScalar friction = 0.5;
    RdScalar restitution = 0.05;

    // Animation
    std::string bonePath = UNDEFINED_STRING;
    std::string boneName = UNDEFINED_STRING;

    // ???
    float maxContactImpulse{ -1.0f };
    float maxDepenetrationVelocity{ -1.0f };
};

struct RagdollJoint {
    RdString name = UNDEFINED_STRING;
    RdString childJsonId = UNDEFINED_STRING;
    RdString parentJsonId = UNDEFINED_STRING;
    RdString childName = UNDEFINED_STRING;
    RdString parentName = UNDEFINED_STRING;
    RdString relativeJsonId = UNDEFINED_STRING;

    // Joint frames
    RdMatrix parentFrame{ RdIdentityInit };
    RdMatrix childFrame{ RdIdentityInit };

    // Angular limits
    RdEulerRotation limitRange;
    RdScalar limitStiffness = 0.0;
    RdScalar limitDampingRatio = 0.0;
    RdBoolean limitAutoOrient = false;
    RdMotion linearMotion = RdMotion::RdMotionLocked;

    // Drive settings
    RdBoolean driveSlerp = false;
    RdEnum    driveSpringType = 0;
    RdScalar  driveLinearStiffness = 0.0;
    RdScalar  driveLinearDamping = 0.0;
    RdScalar  driveAngularStiffness = 0.0;
    RdScalar  driveAngularDamping = 0.0;
    RdVector  driveLinearAmount{ 0,0,0 };
    RdScalar  driveAngularAmountTwist = 0.0;
    RdScalar  driveAngularAmountSwing = 0.0;
    RdScalar  driveMaxLinearForce = -1.0;
    RdScalar  driveMaxAngularForce = -1.0;
    RdBoolean driveWorldspace = false;
    RdMatrix  driveTarget{ RdIdentityInit };
    RdBoolean ignoreMass = false;
    RdBoolean disableCollision = false;
    RdVectorF limitLinear = RdVectorF(0, 0, 0);
};

struct RagdollSolver {
    RdUint positionIterations { 1 };
    RdUint velocityIterations { 1 };
    RdInteger substeps;
    RdVector gravity;
    RdScalar sceneScale{ 1.0 };
    RdScalar linearLimitStiffness;
    RdScalar linearLimitDamping;
    RdScalar angularLimitStiffness;
    RdScalar angularLimitDamping;
};

struct RagdollInfo {
    RagdollSolver m_solver;
    std::vector<RagdollMarker> m_markers;
    std::vector<RagdollJoint> m_joints;

    void PrintJointInfo();
    void PrintMarkerInfo();
    void PrintSolverInfo();

private:
    const RagdollMarker* GetMarkerByName(const RdString& name) const;
};
