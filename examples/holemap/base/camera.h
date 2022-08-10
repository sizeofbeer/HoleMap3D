#pragma once
#include <vector>
#include <string>
#include <unordered_map>

namespace HoleMap {
  class Camera
  {
  public:
    Camera();

    // Access the unique identifier of the camera.
    inline uint32_t CameraId() const;
    inline void SetCameraId(const uint32_t camera_id);

    // Access the camera model.
    inline int ModelId() const;
    inline std::string ModelName() const;
    inline void SetModelId(const int model_id);

    // Access dimensions of the camera sensor.
    inline size_t Width() const;
    inline size_t Height() const;
    inline void SetWidth(const size_t width);
    inline void SetHeight(const size_t height);

    inline const std::vector<double>& Params() const;
    inline std::vector<double>& Params();
    inline double Params(const size_t idx) const;
    inline double& Params(const size_t idx);
    void SetParams(double fx, double fy, double cx, double cy);
    // Convert pixel threshold in image plane to world space.
    double ImageToWorldThreshold(const double threshold) const;

  private:
    // The unique identifier of the camera. If the identifier is not specified
    // it is set to `kInvalidCameraId`.
    uint32_t camera_id_;

    // The identifier of the camera model. If the camera model is not specified
    // the identifier is `kInvalidCameraModelId`.
    int model_id_;
    
    // The dimensions of the image, 0 if not initialized.
    size_t width_;
    size_t height_;

    // 目前支持 SIMPLE_PINHOLE PINHOLE SIMPLE_RADIAL
    // 默认输出都是PINHOLE
    std::string model_name_;
    // 内参依次为 fx, fy, cx, cy
    // The focal length, principal point, and extra parameters. If the camera
    // model is not specified, this vector is empty.
    std::vector<double> params_;
  };

  uint32_t Camera::CameraId() const { return camera_id_; }

  void Camera::SetCameraId(const uint32_t camera_id) { camera_id_ = camera_id; }

  int Camera::ModelId() const { return model_id_; }

  std::string Camera::ModelName() const { return model_name_; }

  void Camera::SetModelId(const int model_id) { model_id_ = model_id; }

  size_t Camera::Width() const { return width_; }

  size_t Camera::Height() const { return height_; }

  void Camera::SetWidth(const size_t width) { width_ = width; }

  void Camera::SetHeight(const size_t height) { height_ = height; }

  const std::vector<double>& Camera::Params() const { return params_; }

  std::vector<double>& Camera::Params() { return params_; }

  double Camera::Params(const size_t idx) const { return params_[idx]; }

  double& Camera::Params(const size_t idx) { return params_[idx]; }
}