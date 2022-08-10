#include "point2d.h"

namespace HoleMap {
    Point2D::Point2D()
        : xy_(Eigen::Vector2d::Zero()), point3D_id_(kInvalidPoint3DId) {}
}
