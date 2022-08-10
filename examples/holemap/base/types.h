#pragma once
#include <eigen/Eigen/Core>


namespace Eigen {
    typedef Eigen::Matrix<uint8_t, 3, 1> Vector3ub;
    typedef Eigen::Matrix<float, 3, 4> Matrix3x4f;
    typedef Eigen::Matrix<double, 3, 4> Matrix3x4d;
}
namespace HoleMap {
    const uint32_t kInvalidCameraId = std::numeric_limits<uint32_t>::max();
    const uint32_t kInvalidImageId = std::numeric_limits<uint32_t>::max();
    const uint32_t kInvalidCameraModelId = std::numeric_limits<uint32_t>::max();
    const uint32_t kInvalidPoint2DIdx = std::numeric_limits<uint32_t>::max();
    const uint64_t kInvalidPoint3DId = std::numeric_limits<uint64_t>::max();
}
