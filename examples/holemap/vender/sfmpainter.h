#pragma once
#include "abcg.hpp"

#include "point3d.h"
#include "camera.h"
#include "image.h"

#include "pointpainter.h"
#include "linepainter.h"
#include "trianglepainter.h"

#include <unordered_map>

namespace HoleMap {

    class SFMPainter {
    public:
        SFMPainter();
        ~SFMPainter();

        void Setup(GLuint program);

        void Upload(std::unordered_map<uint64_t, Point3D> points3D,
                            std::unordered_map<uint32_t, Image> images,
                            std::unordered_map<uint32_t, Camera> cameras);

        void Render(const glm::mat4x4& pmv_matrix, const float point_size, const float line_width);

        void UploadPoseCamera(const Image& image, const Camera& camera,
                            const float image_size, const glm::vec4& plane_color,
                            const glm::vec4& frame_color,
                            std::vector<TrianglePainter::Data>* triangle_data,
                            std::vector<LinePainter::Data>* line_data);
        void UploadPointData(std::unordered_map<uint64_t, Point3D> points3D);
        void UploadPoseData(std::unordered_map<uint32_t, Image> images,
                            std::unordered_map<uint32_t, Camera> cameras);

        void normalize(std::unordered_map<uint64_t, Point3D> points3D);

        inline int getNumGeoms();

    private:
        glm::vec4 mediate_{0.0f, 0.0f, 0.0f, 0.0f};
        LinePainter pose_lines_painter_;
        TrianglePainter pose_triangles_painter_;
        PointPainter points_painter_;
    };

    inline int SFMPainter::getNumGeoms() {
        return points_painter_.getNumGeoms();
    }
}