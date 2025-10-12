#pragma once
#include "Physics/Physics.h"
#include "magnum/MagnumMath.hpp"
#include <filesystem>
#include <string>
#include <unordered_map>

enum struct RdBehaviour {
    kInherit,
    kKinematic,
    kDynamic,
};

enum struct RdConvexDecomposition {
    Off,
    MeshIslands,
    Automatic
};

enum struct RdGeometryType {
    kBox,
    kSphere,
    kCapsule,
    kConvexHull
};

enum struct RdMotion {
    RdMotionInherit,
    RdMotionLocked,
    RdMotionLimited,
    RdMotionFree
};

using RdMatrix3 = Magnum::Math::Matrix3<double>;
using RdMatrix = Magnum::Math::Matrix4<double>;
using RdQuaternion = Magnum::Math::Quaternion<double>;
using RdVector2f = Magnum::Math::Vector2<float>;
using RdVector2i = Magnum::Math::Vector2<int>;
using RdVector = Magnum::Math::Vector3<double>;
using RdVectorF = Magnum::Math::Vector3<float>;
using RdVector2 = Magnum::Math::Vector2<double>;
using RdVector2F = Magnum::Math::Vector2<float>;
using RdVector4 = Magnum::Math::Vector4<double>;
using RdVector4F = Magnum::Math::Vector4<float>;
using RdDegrees = Magnum::Math::Deg<double>;
using RdDegreesF = Magnum::Math::Deg<float>;
using RdRadians = Magnum::Math::Rad<double>;
using RdRadiansF = Magnum::Math::Rad<float>;
using RdColor = Magnum::Math::Color4<float>;
using RdBoolean = bool;
using RdInteger = int;
using RdUint = unsigned int;
using RdPixels = int;
using RdEnum = short;
using RdTime = double;
using RdIndex = std::size_t;
using RdSize = std::size_t;
using RdFrame = int;
using RdSeconds = double;
using RdFloat3 = float[3];
using RdString = std::string;
using RdPath = std::filesystem::path;
using RdEntityId = unsigned;
using RdScalar = double;
using RdLinear = double;
using RdNode = std::size_t;
using RdEdge = std::pair<RdNode, RdNode>;
using RdNodes = std::vector<RdNode>;
using RdEdges = std::vector<RdEdge>;
using RdPoint = RdVector;
using RdReference = void*;

using RdPoints = std::vector<RdPoint>;
using RdVectors = std::vector<RdVector>;
using RdIntegers = std::vector<RdInteger>;
using RdIndices = std::vector<RdIndex>;
using RdColors = std::vector<RdColor>;
using RdUints = std::vector<RdUint>;
using RdPaths = std::vector<RdPath>;
using RdStrings = std::vector<RdString>;

inline auto RdIdentityInit = Magnum::Math::IdentityInit;

struct RdEulerRotation : public RdVector {
    enum RotateOrder : RdEnum {
        eXYZ = 0,
        eOther
    } rotateOrder{ eXYZ };

    using RdVector::RdVector;

    RdQuaternion asQuaternion() const {
        return RdQuaternion::rotation((RdRadians)this->z(), RdVector::zAxis()) *
            RdQuaternion::rotation((RdRadians)this->y(), RdVector::yAxis()) *
            RdQuaternion::rotation((RdRadians)this->x(), RdVector::xAxis());
    }

    RdMatrix asMatrix() const {
        return RdMatrix(this->asQuaternion().toMatrix());
    }
};

inline RdEulerRotation toEulerRotation(const RdQuaternion& quat) {
    const auto euler = quat.toEuler();
    return RdVector(
        static_cast<RdScalar>(euler.x()),
        static_cast<RdScalar>(euler.y()),
        static_cast<RdScalar>(euler.z())
    );
}

inline RdEulerRotation RdToEulerRotation(const RdQuaternion& quat) { 
    return toEulerRotation(quat); 
}

inline RdQuaternion RdEulerToRdQuaternion(const RdEulerRotation& euler) {
    return RdQuaternion::rotation((RdRadians)euler.z(), RdVector::zAxis()) *
           RdQuaternion::rotation((RdRadians)euler.y(), RdVector::yAxis()) *
           RdQuaternion::rotation((RdRadians)euler.x(), RdVector::xAxis());
}

inline PxQuat RdEulerToPxQuat(RdEulerRotation euler) {
    const RdQuaternion quat = RdEulerToRdQuaternion(euler);
    const RdVector vec = quat.vector();
    return PxQuat(
        static_cast<float>(vec.x()),
        static_cast<float>(vec.y()),
        static_cast<float>(vec.z()),
        static_cast<float>(quat.scalar())
    );
}

inline const RdString RdConvexDecompositionTypeToString(RdConvexDecomposition convexDecomposition) {
    switch (convexDecomposition) {
        case RdConvexDecomposition::Off:           return "Off";
        case RdConvexDecomposition::MeshIslands:   return "MeshIslands";
        case RdConvexDecomposition::Automatic:     return "Automatic";
        default:                                   return "Unknown";
    }
}

inline const RdString RdGeometryTypeToString(RdGeometryType type) {
    switch (type) {
        case RdGeometryType::kBox:          return "Box";
        case RdGeometryType::kSphere:       return "Sphere";
        case RdGeometryType::kCapsule:      return "Capsule";
        case RdGeometryType::kConvexHull:   return "ConvexHull";
        default:                            return "Unknown";
    }
}

inline const RdString RdMotionToString(RdMotion motion) {
    switch (motion) {
        case RdMotion::RdMotionInherit:     return "RdMotionInherit";
        case RdMotion::RdMotionLocked:      return "RdMotionLocked";
        case RdMotion::RdMotionLimited:     return "RdMotionLimited";
        case RdMotion::RdMotionFree:        return "RdMotionFree";
        default:                            return "Unknown";
    }
}
inline RdConvexDecomposition StringToRdConvexDecomposition(const RdString& str) {
    if (str == "Off")                   return RdConvexDecomposition::Off;
    if (str == "MeshIslands")           return RdConvexDecomposition::MeshIslands;
    if (str == "Automatic")             return RdConvexDecomposition::Automatic;
    return RdConvexDecomposition::Off;
}

inline RdGeometryType StringToRdGeometryType(const RdString& str) {
    if (str == "Box")                   return RdGeometryType::kBox;
    if (str == "Sphere")                return RdGeometryType::kSphere;
    if (str == "Capsule")               return RdGeometryType::kCapsule;
    if (str == "ConvexHull")            return RdGeometryType::kConvexHull;
    return RdGeometryType::kSphere;
}

inline RdMotion StringToRdMotion(const RdString& str) {
    if (str == "Inherit")               return RdMotion::RdMotionInherit;
    if (str == "Locked")                return RdMotion::RdMotionLocked;
    if (str == "Limited")               return RdMotion::RdMotionLimited;
    if (str == "Free")                  return RdMotion::RdMotionFree;
    return RdMotion::RdMotionLocked;
}

inline glm::mat4 RdMatrixToGlmMat4(const RdMatrix& matrix) {
    glm::mat4 result(1.0f);
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            result[c][r] = matrix[c][r];
        }
    }
    return result;
};

inline physx::PxMat44 RdMatrixToPxMat44(const RdMatrix& matrix) {
    float result[16];
    for (int c = 0; c < 4; ++c)
        for (int r = 0; r < 4; ++r)
            result[c * 4 + r] = static_cast<float>(matrix[c][r]);

    return physx::PxMat44(result);
}

inline std::ostream& operator<<(std::ostream& os, const RdMatrix& matrix) {
    os.setf(std::ios::fixed, std::ios::floatfield);
    os << std::setprecision(6);
    for (int r = 0; r < 4; ++r) {
        os << '[';
        for (int c = 0; c < 4; ++c) {
            os << matrix[c][r]; // Magnum matrices are column major
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