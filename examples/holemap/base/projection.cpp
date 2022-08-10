#include "projection.h"

namespace HoleMap {
  Eigen::Vector4d NormalizeQuaternion(const Eigen::Vector4d& qvec) {
    const double norm = qvec.norm();
    if (norm == 0) {
      // We do not just use (1, 0, 0, 0) because that is a constant and when used
      // for automatic differentiation that would lead to a zero derivative.
      return Eigen::Vector4d(1.0, qvec(1), qvec(2), qvec(3));
    } else {
      return qvec / norm;
    }
  }

  Eigen::Matrix3d QuaternionToRotationMatrix(const Eigen::Vector4d& qvec) {
    const Eigen::Vector4d normalized_qvec = NormalizeQuaternion(qvec);
    const Eigen::Quaterniond quat(normalized_qvec(0), normalized_qvec(1),
                                  normalized_qvec(2), normalized_qvec(3));
    return quat.toRotationMatrix();
  }

  

  Eigen::Vector3d ProjectionCenterFromMatrix(
      const Eigen::Matrix3x4d& proj_matrix) {
    return -proj_matrix.leftCols<3>().transpose() * proj_matrix.rightCols<1>();
  }

  Eigen::Matrix3x4d ComposeProjectionMatrix(const Eigen::Vector4d& qvec,
                                            const Eigen::Vector3d& tvec) {
    Eigen::Matrix3x4d proj_matrix;
    proj_matrix.leftCols<3>() = QuaternionToRotationMatrix(qvec);
    proj_matrix.rightCols<1>() = tvec;
    return proj_matrix;
  }

  Eigen::Matrix3x4d InvertProjectionMatrix(const Eigen::Matrix3x4d& proj_matrix) {
    Eigen::Matrix3x4d inv_proj_matrix;
    inv_proj_matrix.leftCols<3>() = proj_matrix.leftCols<3>().transpose();
    inv_proj_matrix.rightCols<1>() = ProjectionCenterFromMatrix(proj_matrix);
    return inv_proj_matrix;
  }
}