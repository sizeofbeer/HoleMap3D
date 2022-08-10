#include "sfmpainter.h"
#include "params.h"

namespace HoleMap {

    SFMPainter::SFMPainter() {}

    SFMPainter::~SFMPainter()
    {
        points_painter_.~PointPainter();
        pose_lines_painter_.~LinePainter();
        pose_triangles_painter_.~TrianglePainter();
    }

    void SFMPainter::Setup(GLuint program)
    {
        points_painter_.Setup(program);
        pose_lines_painter_.Setup(program);
        pose_triangles_painter_.Setup(program);
    }

    void SFMPainter::Upload(std::unordered_map<uint64_t, Point3D> points3D,
                            std::unordered_map<uint32_t, Image> images,
                            std::unordered_map<uint32_t, Camera> cameras)
    {
        UploadPointData(points3D);
        UploadPoseData(images, cameras);
    }

    void SFMPainter::Render(const glm::mat4x4& pmv_matrix,
                            const float point_size, const float line_width)
    {
        points_painter_.Render(pmv_matrix, point_size);
        pose_lines_painter_.Render(pmv_matrix, line_width);
        pose_triangles_painter_.Render(pmv_matrix);
    }

    void SFMPainter::normalize(std::unordered_map<uint64_t, Point3D> points3D)
    {
        glm::vec4 coord{0.0f, 0.0f, 0.0f, 0.0f};
        // 求中心坐标
        glm::vec3 max(std::numeric_limits<float>::lowest());
        glm::vec3 min(std::numeric_limits<float>::max());
        for (const auto& point3D : points3D) {
            mediate_[0] += static_cast<float>(point3D.second.XYZ(0));
            mediate_[1] += static_cast<float>(point3D.second.XYZ(1));
            mediate_[2] += static_cast<float>(point3D.second.XYZ(2));
            max.x = std::max(max.x, static_cast<float>(point3D.second.XYZ(0)));
            max.y = std::max(max.y, static_cast<float>(point3D.second.XYZ(1)));
            max.z = std::max(max.z, static_cast<float>(point3D.second.XYZ(2)));
            min.x = std::min(min.x, static_cast<float>(point3D.second.XYZ(0)));
            min.y = std::min(min.y, static_cast<float>(point3D.second.XYZ(1)));
            min.z = std::min(min.z, static_cast<float>(point3D.second.XYZ(2)));
        }
        // Center
        mediate_[0] /= points3D.size();
        mediate_[1] /= points3D.size();
        mediate_[2] /= points3D.size();
        mediate_[3] = 2.0f / glm::length(max - min) / 2;
    }

    void SFMPainter::UploadPoseCamera(const Image& image, const Camera& camera,
                                    const float image_size, const glm::vec4& plane_color,
                                    const glm::vec4& frame_color,
                                    std::vector<TrianglePainter::Data>* triangle_data,
                                    std::vector<LinePainter::Data>* line_data)
    {
        // Generate camera dimensions in OpenGL (world) coordinate space.
        const float kBaseCameraWidth = 1024.0f;
        const float image_width = image_size * camera.Width() / kBaseCameraWidth;
        const float image_height = image_width * static_cast<float>(camera.Height()) /
                                static_cast<float>(camera.Width());
        const float image_extent = std::max(image_width, image_height);
        const float camera_extent = std::max(camera.Width(), camera.Height());
        const float camera_extent_world =
            static_cast<float>(camera.ImageToWorldThreshold(camera_extent));
        const float focal_length = 2.0f * image_extent / camera_extent_world;

        const Eigen::Matrix<float, 3, 4> inv_proj_matrix =
            image.InverseProjectionMatrix().cast<float>();

        const Eigen::Vector3f pc = (inv_proj_matrix.rightCols<1>() -
                        Eigen::Vector3f(mediate_[0], mediate_[1], mediate_[2])) * mediate_[3];
        const Eigen::Vector3f tl = (inv_proj_matrix *
                        Eigen::Vector4f(image_width, -image_height, focal_length, 1) -
                        Eigen::Vector3f(mediate_[0], mediate_[1], mediate_[2])) * mediate_[3];
        const Eigen::Vector3f tr = (inv_proj_matrix *
                        Eigen::Vector4f(image_width, image_height, focal_length, 1) -
                        Eigen::Vector3f(mediate_[0], mediate_[1], mediate_[2])) * mediate_[3];
        const Eigen::Vector3f br = (inv_proj_matrix *
                        Eigen::Vector4f(-image_width, image_height, focal_length, 1) -
                        Eigen::Vector3f(mediate_[0], mediate_[1], mediate_[2])) * mediate_[3];
        const Eigen::Vector3f bl = (inv_proj_matrix *
                        Eigen::Vector4f(-image_width, -image_height, focal_length, 1) -
                        Eigen::Vector3f(mediate_[0], mediate_[1], mediate_[2])) * mediate_[3];

        // Image plane as two triangles.
        if (line_data != nullptr)
        {
            triangle_data->emplace_back(
                PointPainter::Data(tl(0), tl(1), tl(2), plane_color[0], plane_color[1],
                                plane_color[2], plane_color[3]),
                PointPainter::Data(tr(0), tr(1), tr(2), plane_color[0], plane_color[1],
                                plane_color[2], plane_color[3]),
                PointPainter::Data(bl(0), bl(1), bl(2), plane_color[0], plane_color[1],
                                plane_color[2], plane_color[3]));

            triangle_data->emplace_back(
                PointPainter::Data(bl(0), bl(1), bl(2), plane_color[0], plane_color[1],
                                plane_color[2], plane_color[3]),
                PointPainter::Data(tr(0), tr(1), tr(2), plane_color[0], plane_color[1],
                                plane_color[2], plane_color[3]),
                PointPainter::Data(br(0), br(1), br(2), plane_color[0], plane_color[1],
                                plane_color[2], plane_color[3]));
        }

        // Frame around image plane and connecting lines to projection center.
        if (line_data != nullptr)
        {
            line_data->emplace_back(
                PointPainter::Data(pc(0), pc(1), pc(2), frame_color[0], frame_color[1],
                                    frame_color[2], frame_color[3]),
                PointPainter::Data(tl(0), tl(1), tl(2), frame_color[0], frame_color[1],
                                    frame_color[2], frame_color[3]));

            line_data->emplace_back(
                PointPainter::Data(pc(0), pc(1), pc(2), frame_color[0], frame_color[1],
                                    frame_color[2], frame_color[3]),
                PointPainter::Data(tr(0), tr(1), tr(2), frame_color[0], frame_color[1],
                                    frame_color[2], frame_color[3]));

            line_data->emplace_back(
                PointPainter::Data(pc(0), pc(1), pc(2), frame_color[0], frame_color[1],
                                    frame_color[2], frame_color[3]),
                PointPainter::Data(br(0), br(1), br(2), frame_color[0], frame_color[1],
                                    frame_color[2], frame_color[3]));

            line_data->emplace_back(
                PointPainter::Data(pc(0), pc(1), pc(2), frame_color[0], frame_color[1],
                                    frame_color[2], frame_color[3]),
                PointPainter::Data(bl(0), bl(1), bl(2), frame_color[0], frame_color[1],
                                    frame_color[2], frame_color[3]));

            line_data->emplace_back(
                PointPainter::Data(tl(0), tl(1), tl(2), frame_color[0], frame_color[1],
                                    frame_color[2], frame_color[3]),
                PointPainter::Data(tr(0), tr(1), tr(2), frame_color[0], frame_color[1],
                                    frame_color[2], frame_color[3]));

            line_data->emplace_back(
                PointPainter::Data(tr(0), tr(1), tr(2), frame_color[0], frame_color[1],
                                    frame_color[2], frame_color[3]),
                PointPainter::Data(br(0), br(1), br(2), frame_color[0], frame_color[1],
                                    frame_color[2], frame_color[3]));

            line_data->emplace_back(
                PointPainter::Data(br(0), br(1), br(2), frame_color[0], frame_color[1],
                                    frame_color[2], frame_color[3]),
                PointPainter::Data(bl(0), bl(1), bl(2), frame_color[0], frame_color[1],
                                    frame_color[2], frame_color[3]));

            line_data->emplace_back(
                PointPainter::Data(bl(0), bl(1), bl(2), frame_color[0], frame_color[1],
                                    frame_color[2], frame_color[3]),
                PointPainter::Data(tl(0), tl(1), tl(2), frame_color[0], frame_color[1],
                                    frame_color[2], frame_color[3]));
        }
    }

    void SFMPainter::UploadPoseData(std::unordered_map<uint32_t, Image> images,
                                    std::unordered_map<uint32_t, Camera> cameras)
    {
        std::vector<LinePainter::Data> line_data;
        line_data.reserve(8 * images.size());

        std::vector<TrianglePainter::Data> triangle_data;
        triangle_data.reserve(2 * images.size());

        for (const auto& image : images) {
            const Camera& camera = cameras[image.second.CameraId()];

            UploadPoseCamera(image.second, camera, 0.02f,
                            kSelectedImagePlaneColor, kSelectedImageFrameColor,
                            &triangle_data, &line_data);
        }
        pose_lines_painter_.Upload(line_data);
        pose_triangles_painter_.Upload(triangle_data);
    }

    void SFMPainter::UploadPointData(std::unordered_map<uint64_t, Point3D> points3D)
    {
        std::vector<PointPainter::Data> data;
        normalize(points3D);
        
        // Assume we want to display the majority of points
        data.reserve(points3D.size());
        for (const auto& point3D : points3D) {
            PointPainter::Data painter_point;

            painter_point.x = (static_cast<float>(point3D.second.XYZ(0)) - mediate_[0]) * mediate_[3];
            painter_point.y = (static_cast<float>(point3D.second.XYZ(1)) - mediate_[1]) * mediate_[3];
            painter_point.z = (static_cast<float>(point3D.second.XYZ(2)) - mediate_[2]) * mediate_[3];

            painter_point.r = static_cast<float>(point3D.second.Color(0) / 255.0f);
            painter_point.g = static_cast<float>(point3D.second.Color(1) / 255.0f);
            painter_point.b = static_cast<float>(point3D.second.Color(2) / 255.0f);
            painter_point.a = 0.4f;

            data.push_back(painter_point);
        }
        points_painter_.Upload(data);
    }
}