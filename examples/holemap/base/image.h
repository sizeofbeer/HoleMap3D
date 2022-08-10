#pragma once
#include "camera.h"
#include "point2d.h"
#include "types.h"
#include "projection.h"

#include <string>
#include <vector>
#include <eigen/Eigen/Core>
#include <eigen/Eigen/Geometry>

namespace HoleMap {
  class Image {
  public:

    Image();
    
    // Access the unique identifier of the image.
    inline uint32_t ImageId() const;
    inline void SetImageId(const uint32_t image_id);

    // Access the name of the image.
    inline const std::string& Name() const;
    inline std::string& Name();
    inline void SetName(const std::string& name);

    // Access the unique identifier of the camera. Note that multiple images
    // might share the same camera.
    inline uint32_t CameraId() const;
    inline void SetCameraId(const uint32_t camera_id);

    // Get the number of image points.
    inline uint32_t NumPoints2D() const;

    // Get the number of triangulations, i.e. the number of points that
    // are part of a 3D point track.
    inline uint32_t NumPoints3D() const;
    inline void SetNumPoints3D(const uint32_t num_points3D);

    // Access quaternion vector as (qw, qx, qy, qz) specifying the rotation of the
    // pose which is defined as the transformation from world to image space.
    inline const Eigen::Vector4d& Qvec() const;
    inline Eigen::Vector4d& Qvec();
    inline double Qvec(const size_t idx) const;
    inline double& Qvec(const size_t idx);
    inline void SetQvec(const Eigen::Vector4d& qvec);
    
    // 四元数转旋转矩阵(互)
    void Qvec2Rotate();
    void Rotate2Qvec();
    inline const Eigen::Matrix3d& Rotate() const;
    inline Eigen::Matrix3d& Rotate();
    inline double Rotate(const size_t idx, const size_t idy) const;
    inline double& Rotate(const size_t idx, const size_t idy);
    inline void SetRotate(const Eigen::Matrix3d& rotate);
    // 平移向量+旋转矩阵求中心坐标
    // C = -R^T t
    void Tvec2Center();
    // t = -R C
    void Center2Tvec();
    inline const Eigen::Vector3d& Center() const;
    inline Eigen::Vector3d& Center();
    inline double Center(const size_t idx) const;
    inline double& Center(const size_t idx);
    inline void SetCenter(const Eigen::Vector3d& center);
    
    // Access translation vector as (tx, ty, tz) specifying the translation of the
    // pose which is defined as the transformation from world to image space.
    inline const Eigen::Vector3d& Tvec() const;
    inline Eigen::Vector3d& Tvec();
    inline double Tvec(const size_t idx) const;
    inline double& Tvec(const size_t idx);
    inline void SetTvec(const Eigen::Vector3d& tvec);

    // Access the coordinates of image points.
    inline const class Point2D& Point2D(const uint32_t point2D_idx) const;
    inline class Point2D& Point2D(const uint32_t point2D_idx);
    inline const std::vector<class Point2D>& Points2D() const;
    void SetPoints2D(const std::vector<Eigen::Vector2d>& points);
    void SetPoints2D(const std::vector<class Point2D>& points);
    void AddPoint2D(class Point2D& point);

    // Set the point as triangulated, i.e. it is part of a 3D point track.
    void SetPoint3DForPoint2D(const uint32_t point2D_idx,
                              const uint64_t point3D_id);

    // Normalize the quaternion vector.
    void NormalizeQvec();

    Eigen::Matrix3x4d InverseProjectionMatrix() const;

  private:
    // Identifier of the image, if not specified `kInvalidImageId`.
    uint32_t image_id_;

    // The name of the image, i.e. the relative path.
    std::string name_;

    // The identifier of the associated camera. Note that multiple images might
    // share the same camera. If not specified `kInvalidCameraId`.
    uint32_t camera_id_;

    // The number of 3D points the image observes, i.e. the sum of its `points2D`
    // where `point3D_id != kInvalidPoint3DId`.
    uint32_t num_points3D_;

    // The pose of the image, defined as the transformation from world to image.
    Eigen::Vector4d qvec_;
    Eigen::Matrix3d rotate_;
    Eigen::Vector3d tvec_;
    Eigen::Vector3d center_;

    // All image points, including points that are not part of a 3D point track.
    std::vector<class Point2D> points2D_;

};

////////////////////////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////////////////////////

  uint32_t Image::ImageId() const { return image_id_; }

  void Image::SetImageId(const uint32_t image_id) { image_id_ = image_id; }

  const std::string& Image::Name() const { return name_; }

  std::string& Image::Name() { return name_; }

  void Image::SetName(const std::string& name) { name_ = name; }

  inline uint32_t Image::CameraId() const { return camera_id_; }

  inline void Image::SetCameraId(const uint32_t camera_id) {
    camera_id_ = camera_id;
  }

  uint32_t Image::NumPoints2D() const {
    return static_cast<uint32_t>(points2D_.size());
  }

  uint32_t Image::NumPoints3D() const { return num_points3D_; }

  void Image::SetNumPoints3D(const uint32_t num_points3D) { num_points3D_ = num_points3D; }

  const Eigen::Vector4d& Image::Qvec() const { return qvec_; }

  Eigen::Vector4d& Image::Qvec() { return qvec_; }

  inline double Image::Qvec(const size_t idx) const { return qvec_(idx); }

  inline double& Image::Qvec(const size_t idx) { return qvec_(idx); }

  void Image::SetQvec(const Eigen::Vector4d& qvec) { qvec_ = qvec; }

  const Eigen::Vector3d& Image::Tvec() const { return tvec_; }

  Eigen::Vector3d& Image::Tvec() { return tvec_; }

  inline double Image::Tvec(const size_t idx) const { return tvec_(idx); }

  inline double& Image::Tvec(const size_t idx) { return tvec_(idx); }

  void Image::SetTvec(const Eigen::Vector3d& tvec) { tvec_ = tvec; }

  const class Point2D& Image::Point2D(const uint32_t point2D_idx) const {
    return points2D_.at(point2D_idx);
  }

  class Point2D& Image::Point2D(const uint32_t point2D_idx) {
    return points2D_.at(point2D_idx);
  }

  const std::vector<class Point2D>& Image::Points2D() const { return points2D_; }

  const Eigen::Matrix3d& Image::Rotate() const { return rotate_; }

  Eigen::Matrix3d& Image::Rotate() { return rotate_; }

  inline double Image::Rotate(const size_t idx, const size_t idy) const { return rotate_(idx, idy); }

  inline double& Image::Rotate(const size_t idx, const size_t idy) { return rotate_(idx, idy); }

  void Image::SetRotate(const Eigen::Matrix3d& rotate) { rotate_ = rotate; }

  const Eigen::Vector3d& Image::Center() const { return center_; }

  Eigen::Vector3d& Image::Center() { return center_; }

  inline double Image::Center(const size_t idx) const { return center_(idx); }

  inline double& Image::Center(const size_t idx) { return center_(idx); }

  void Image::SetCenter(const Eigen::Vector3d& center) { center_ = center; }
}