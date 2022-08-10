#include "image.h"

#include <cmath>

namespace HoleMap {

  Image::Image()
      : image_id_(kInvalidImageId),
        name_(""),
        camera_id_(kInvalidCameraId),
        num_points3D_(0),
        qvec_(1.0, 0.0, 0.0, 0.0),
        tvec_(0.0, 0.0, 0.0) {}

  void Image::SetPoints2D(const std::vector<Eigen::Vector2d>& points) {
    points2D_.resize(points.size());
    for (uint32_t point2D_idx = 0; point2D_idx < points.size(); ++point2D_idx) {
      points2D_[point2D_idx].SetXY(points[point2D_idx]);
    }
  }

  void Image::SetPoints2D(const std::vector<class Point2D>& points) {
    points2D_ = points;
  }

  void Image::AddPoint2D(class Point2D& point){
    points2D_.push_back(point);
  }

  void Image::SetPoint3DForPoint2D(const uint32_t point2D_idx,
                                  const uint64_t point3D_id) {
    class Point2D& point2D = points2D_.at(point2D_idx);
    if (!point2D.HasPoint3D()) {
      num_points3D_ += 1;
    }
    point2D.SetPoint3DId(point3D_id);
  }

  void Image::NormalizeQvec() { 
    const double norm = qvec_.norm();
    if (norm == 0){
      qvec_ = Eigen::Vector4d(1.0, 0.0, 0.0, 0.0);
    } else {
      qvec_ = qvec_ / norm;
    }
  }

  void Image::Qvec2Rotate() {
    double qw = qvec_[0];
    double qx = qvec_[1];
    double qy = qvec_[2];
    double qz = qvec_[3];
    Eigen::Matrix3d rotate;
    rotate(0, 0) = qw * qw + qx * qx - qz * qz - qy * qy;
    rotate(0, 1) = 2 * qx * qy - 2 * qz * qw;
    rotate(0, 2) = 2 * qy * qw + 2 * qz * qx;
    rotate(1, 0) = 2 * qx * qy + 2 * qw * qz;
    rotate(1, 1) = qy * qy + qw * qw - qz * qz - qx * qx;
    rotate(1, 2) = 2 * qz * qy - 2 * qx * qw;
    rotate(2, 0) = 2 * qx * qz - 2 * qy * qw;
    rotate(2, 1) = 2 * qy * qz + 2 * qw * qx;
    rotate(2, 2) = qz * qz + qw * qw - qy * qy - qx * qx;
    rotate_ = rotate;
  }

  void Image::Rotate2Qvec(){
    Eigen::Vector4d qvec;
    qvec[0] = 1 + rotate_(0, 0) + rotate_(1, 1) + rotate_(2, 2);
    if (qvec[0] > 0.000000001) {
      qvec[0] = std::sqrt(qvec[0]) / 2.0;
      qvec[1] = (rotate_(2, 1) - rotate_(1, 2)) / (4.0 * qvec[0]);
      qvec[2] = (rotate_(0, 2) - rotate_(2, 0)) / (4.0 * qvec[0]);
      qvec[3] = (rotate_(1, 0) - rotate_(0, 1)) / (4.0 * qvec[0]);
    } else {
      if (rotate_(0, 0) > rotate_(1, 1) && rotate_(0, 0) > rotate_(2, 2)) {
        auto s = 2.0 * std::sqrt(1.0 + rotate_(0, 0) - rotate_(1, 1) - rotate_(2, 2));
        qvec[1] = 0.25 * s;
        qvec[2] = (rotate_(0, 1) + rotate_(1, 0)) / s;
        qvec[3] = (rotate_(0, 2) + rotate_(2, 0)) / s;
        qvec[0] = (rotate_(1, 2) - rotate_(2, 1)) / s;
      } else if (rotate_(1, 1) > rotate_(2, 2)) {
        auto s = 2.0 * std::sqrt(1.0 + rotate_(1, 1) - rotate_(0, 0) - rotate_(2, 2));
        qvec[1] = (rotate_(0, 1) + rotate_(1, 0)) / s;
        qvec[2] = 0.25 * s;
        qvec[3] = (rotate_(1, 2) + rotate_(2, 1)) / s;
        qvec[0] = (rotate_(0, 2) - rotate_(2, 0)) / s;
      } else {
        auto s = 2.0 * std::sqrt(1.0 + rotate_(2, 2) - rotate_(0, 0) - rotate_(1, 1));
        qvec[1] = (rotate_(0, 2) + rotate_(2, 0)) / s;
        qvec[2] = (rotate_(1, 2) + rotate_(2, 1)) / s;
        qvec[3] = 0.25 * s;
        qvec[0] = (rotate_(0, 1) - rotate_(1, 0)) / s;
      }      
    }
    qvec_ = qvec;
  }

  // C = -R^T t
  void Image::Tvec2Center(){
    center_ = - rotate_.reverse() * tvec_;
  }
    // t = -R C
  void Image::Center2Tvec(){
    tvec_ = - rotate_ * center_;
  }

  Eigen::Matrix3x4d Image::InverseProjectionMatrix() const {
    return InvertProjectionMatrix(ComposeProjectionMatrix(qvec_, tvec_));
  }
}