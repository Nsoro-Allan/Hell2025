#pragma once
#include "Types.h"
#include <iostream>
#include "Util.h"

inline std::ostream& operator<<(std::ostream& os, const RdMatrix& m) {
    os.setf(std::ios::fixed, std::ios::floatfield);
    os << std::setprecision(6);
    for (int r = 0; r < 4; ++r) {
        os << '[';
        for (int c = 0; c < 4; ++c) {
            os << m[c][r]; // Magnum matrices are column major
            if (c < 3) os << ' ';
        }
        os << ']';
        if (r < 3) os << '\n';
    }
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const RdVector& v) {
    os.setf(std::ios::fixed, std::ios::floatfield);
    os << std::setprecision(6)
        << '[' << v[0] << ' ' << v[1] << ' ' << v[2] << ']';
    return os;
}

inline std::string ToString(const RdVector& v) {
    std::ostringstream ss;
    ss << v;
    return ss.str();
}

struct RagdollMarker {
    // Transforms
    RdString name = UNDEFINED_STRING;
    RdMatrix inputMatrix{ RdIdentityInit };
    RdMatrix originMatrix{ RdIdentityInit };
    RdMatrix parentFrame{ RdIdentityInit };
    RdMatrix childFrame{ RdIdentityInit };

    // Geometry
    RdString shapeType = UNDEFINED_STRING;
    RdEulerRotation shapeRotation{};
    RdVector extents{ 0, 0, 0 };
    RdVector shapeOffset{ 0, 0, 0 };
    RdLinear shapeRadius{ 0.0 };
    RdLinear shapeLength{ 0.0 };
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
    std::string bonePath;
    std::string boneName;
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
    RdString linearMotionString = UNDEFINED_STRING;
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
    RdInteger positionIterations;
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

    void PrintJointInfo(const RagdollJoint& joint);     // fix to not take in a param
    void PrintMarkerInfo(const RagdollMarker& marker);  // fix to not take in a param
    void PrintSolverInfo();

private:
    const RagdollMarker* GetMarkerByName(const RdString& name) const;
};
