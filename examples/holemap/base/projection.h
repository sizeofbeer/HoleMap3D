#pragma once
#include <limits>
#include <vector>

#include <eigen/Eigen/Core>
#include <eigen/Eigen/Geometry>
#include "types.h"


namespace HoleMap {
    Eigen::Vector4d NormalizeQuaternion(const Eigen::Vector4d& qvec);
      
    Eigen::Matrix3d QuaternionToRotationMatrix(const Eigen::Vector4d& qvec);

    Eigen::Vector3d ProjectionCenterFromMatrix(const Eigen::Matrix3x4d& proj_matrix);

    Eigen::Matrix3x4d ComposeProjectionMatrix(const Eigen::Vector4d& qvec,
                                            const Eigen::Vector3d& tvec);

    Eigen::Matrix3x4d InvertProjectionMatrix(const Eigen::Matrix3x4d& proj_matrix);
}