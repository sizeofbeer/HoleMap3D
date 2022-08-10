#include "camera.h"
#include "types.h"

namespace HoleMap {
  Camera::Camera()
      : camera_id_(kInvalidCameraId),
        model_id_(kInvalidCameraModelId),
        width_(0),
        height_(0),
        model_name_{"PINHOLE"},
        params_{0.0, 0.0, 0.0, 0.0} {}
  
  void Camera::SetParams(double fx, double fy, double cx, double cy) {
    params_[0] = fx;
    params_[1] = fy;
    params_[2] = cx;
    params_[3] = cy;
  }

  double Camera::ImageToWorldThreshold(const double threshold) const {
    return threshold / params_[0];
  }
}
