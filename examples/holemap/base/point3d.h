#pragma once
#include "track.h"

#include <vector>
#include <eigen/Eigen/Core>

namespace HoleMap {
  // 3D point class that holds information about triangulated 2D points.
  class Point3D {
  public:
  
    Point3D();

    // The point coordinate in world space.
    inline const Eigen::Vector3d& XYZ() const;
    inline Eigen::Vector3d& XYZ();
    inline double XYZ(const size_t idx) const;
    inline double& XYZ(const size_t idx);
    inline double X() const;
    inline double Y() const;
    inline double Z() const;
    inline void SetXYZ(const Eigen::Vector3d& xyz);

    // The RGB color of the point.
    inline const Eigen::Vector3ub& Color() const;
    inline Eigen::Vector3ub& Color();
    inline uint8_t Color(const size_t idx) const;
    inline uint8_t& Color(const size_t idx);
    inline void SetColor(const Eigen::Vector3ub& color);

    // The mean reprojection error in image space.
    // 默认为0.0
    inline double Error() const;
    inline void SetError(const double error);

    inline const class Track& Track() const;
    inline class Track& Track();
    inline void SetTrack(const class Track& track);

  private:
    // The 3D position of the point.
    Eigen::Vector3d xyz_;

    // The color of the point in the range [0, 255].
    Eigen::Vector3ub color_;

    // The mean reprojection error in pixels.
    double error_;

    // The track of the point as a list of image observations.
    class Track track_;
};

////////////////////////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////////////////////////

  const Eigen::Vector3d& Point3D::XYZ() const { return xyz_; }

  Eigen::Vector3d& Point3D::XYZ() { return xyz_; }

  double Point3D::XYZ(const size_t idx) const { return xyz_(idx); }

  double& Point3D::XYZ(const size_t idx) { return xyz_(idx); }

  double Point3D::X() const { return xyz_.x(); }

  double Point3D::Y() const { return xyz_.y(); }

  double Point3D::Z() const { return xyz_.z(); }

  void Point3D::SetXYZ(const Eigen::Vector3d& xyz) { xyz_ = xyz; }

  const Eigen::Vector3ub& Point3D::Color() const { return color_; }

  Eigen::Vector3ub& Point3D::Color() { return color_; }

  uint8_t Point3D::Color(const size_t idx) const { return color_(idx); }

  uint8_t& Point3D::Color(const size_t idx) { return color_(idx); }

  void Point3D::SetColor(const Eigen::Vector3ub& color) { color_ = color; }

  double Point3D::Error() const { return error_; }

  void Point3D::SetError(const double error) { error_ = error; }

  const class Track& Point3D::Track() const { return track_; }

  class Track& Point3D::Track() {
    return track_;
  }

  void Point3D::SetTrack(const class Track& track) { track_ = track; }

}
