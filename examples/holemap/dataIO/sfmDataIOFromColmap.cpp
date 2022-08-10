#include "sfmDataIOFromColmap.hpp"
#include "endian.hpp"

#include <ios>
#include <fstream>
#include <vector>
#include <eigen/Eigen/Core>

namespace HoleMap
{
    void ColmapSFMDataIO::ReadCamerasBinary(const std::string& path) {
        std::ifstream file(path, std::ios::binary);

        const size_t num_cameras = ReadBinaryLittleEndian<uint64_t>(&file);
        for (size_t i = 0; i < num_cameras; ++i) {
            class Camera camera;
            camera.SetCameraId(ReadBinaryLittleEndian<uint32_t>(&file));
            camera.SetModelId(ReadBinaryLittleEndian<int>(&file));
            camera.SetWidth(ReadBinaryLittleEndian<uint64_t>(&file));
            camera.SetHeight(ReadBinaryLittleEndian<uint64_t>(&file));
            ReadBinaryLittleEndian<double>(&file, &camera.Params());
            cameras_.emplace(camera.CameraId(), camera);
        }
    }

    void ColmapSFMDataIO::ReadImagesBinary(const std::string& path) {
        std::ifstream file(path, std::ios::binary);

        const size_t num_reg_images = ReadBinaryLittleEndian<uint64_t>(&file);
        for (size_t i = 0; i < num_reg_images; ++i) {
            class Image image;

            image.SetImageId(ReadBinaryLittleEndian<uint32_t>(&file));

            image.Qvec(0) = ReadBinaryLittleEndian<double>(&file);
            image.Qvec(1) = ReadBinaryLittleEndian<double>(&file);
            image.Qvec(2) = ReadBinaryLittleEndian<double>(&file);
            image.Qvec(3) = ReadBinaryLittleEndian<double>(&file);
            image.NormalizeQvec();
            image.Qvec2Rotate();

            image.Tvec(0) = ReadBinaryLittleEndian<double>(&file);
            image.Tvec(1) = ReadBinaryLittleEndian<double>(&file);
            image.Tvec(2) = ReadBinaryLittleEndian<double>(&file);
            image.Tvec2Center();

            image.SetCameraId(ReadBinaryLittleEndian<uint32_t>(&file));

            char name_char;
            do {
                file.read(&name_char, 1);
                if (name_char != '\0') {
                    image.Name() += name_char;
                }
            } while (name_char != '\0');

            const size_t num_points2D = ReadBinaryLittleEndian<uint64_t>(&file);

            std::vector<Eigen::Vector2d> points2D;
            points2D.reserve(num_points2D);
            std::vector<uint64_t> point3D_ids;
            point3D_ids.reserve(num_points2D);
            for (size_t j = 0; j < num_points2D; ++j) {
                const double x = ReadBinaryLittleEndian<double>(&file);
                const double y = ReadBinaryLittleEndian<double>(&file);
                points2D.emplace_back(x, y);
                point3D_ids.push_back(ReadBinaryLittleEndian<uint64_t>(&file));
            }

            image.SetPoints2D(points2D);

            for (uint32_t point2D_idx = 0; point2D_idx < image.NumPoints2D(); ++point2D_idx) {
                if (point3D_ids[point2D_idx] != kInvalidPoint3DId) {
                    image.SetPoint3DForPoint2D(point2D_idx, point3D_ids[point2D_idx]);
                }
            }

            images_.emplace(image.ImageId(), image);
        }
    }

    void ColmapSFMDataIO::ReadPoints3DBinary(const std::string& path) {
        std::ifstream file(path, std::ios::binary);

        const size_t num_points3D = ReadBinaryLittleEndian<uint64_t>(&file);
        for (size_t i = 0; i < num_points3D; ++i) {
            class Point3D point3D;

            const uint64_t point3D_id = ReadBinaryLittleEndian<uint64_t>(&file);

            point3D.XYZ()(0) = ReadBinaryLittleEndian<double>(&file);
            point3D.XYZ()(1) = ReadBinaryLittleEndian<double>(&file);
            point3D.XYZ()(2) = ReadBinaryLittleEndian<double>(&file);
            point3D.Color(0) = ReadBinaryLittleEndian<uint8_t>(&file);
            point3D.Color(1) = ReadBinaryLittleEndian<uint8_t>(&file);
            point3D.Color(2) = ReadBinaryLittleEndian<uint8_t>(&file);
            // ingore error
            ReadBinaryLittleEndian<double>(&file);

            const size_t track_length = ReadBinaryLittleEndian<uint64_t>(&file);
            for (size_t j = 0; j < track_length; ++j) {
                const uint32_t image_id = ReadBinaryLittleEndian<uint32_t>(&file);
                const uint32_t point2D_idx = ReadBinaryLittleEndian<uint32_t>(&file);
                point3D.Track().AddElement(image_id, point2D_idx);
            }
            point3D.Track().Compress();

            points3D_.emplace(point3D_id, point3D);
        }
    }

    void ColmapSFMDataIO::WriteCamerasText(const std::string& path) const {
        std::ofstream file(path, std::ios::trunc);

        // Ensure that we don't loose any precision by storing in text.
        file.precision(17);

        file << "# Camera list with one line of data per camera:" << std::endl;
        file << "#   CAMERA_ID, MODEL, WIDTH, HEIGHT, PARAMS[]" << std::endl;
        file << "# Number of cameras: " << cameras_.size() << std::endl;

        for (const auto& camera : cameras_) {
            std::ostringstream line;
            line.precision(17);

            line << camera.first << " ";
            line << camera.second.ModelName() << " ";
            line << camera.second.Width() << " ";
            line << camera.second.Height() << " ";
            const std::vector<double> camParams = camera.second.Params();
            if (camera.second.ModelId() > 2){
                std::cout << "本系统不支持该相机模型!" << std::endl;
                return;
            } else if (camera.second.ModelId() == 0 || camera.second.ModelId() == 2){
                line << camParams[0] << " ";
                line << camParams[0] << " ";
                line << camParams[1] << " ";
                line << camParams[2] << " ";
            } else {
                for (const double param : camParams) {
                    line << param << " ";
                }
            }

            std::string line_string = line.str();
            line_string = line_string.substr(0, line_string.size() - 1);

            file << line_string << std::endl;
        }
    }

    void ColmapSFMDataIO::WriteImagesText(const std::string& path) const {
        std::ofstream file(path, std::ios::trunc);
        
        // Ensure that we don't loose any precision by storing in text.
        file.precision(17);

        file << "# Image list with two lines of data per image:" << std::endl;
        file << "#   IMAGE_ID, QW, QX, QY, QZ, TX, TY, TZ, CAMERA_ID, "
                "NAME"
            << std::endl;
        file << "#   POINTS2D[] as (X, Y, POINT3D_ID)" << std::endl;
        file << "# Number of images: " << images_.size() << std::endl;

        for (const auto& image : images_) {
            std::ostringstream line;
            line.precision(17);

            std::string line_string;

            line << image.first << " ";

            // QVEC (qw, qx, qy, qz)
            const Eigen::Vector4d unnormalized_qvec = image.second.Qvec();
            Eigen::Vector4d normalized_qvec {0.0, 0.0, 0.0, 0.0};
            const double norm = unnormalized_qvec.norm();
            if (norm == 0){
                normalized_qvec = Eigen::Vector4d(
                    1.0, unnormalized_qvec(1), unnormalized_qvec(2), unnormalized_qvec(3));
            } else {
                normalized_qvec = unnormalized_qvec / norm;
            }
            line << normalized_qvec(0) << " ";
            line << normalized_qvec(1) << " ";
            line << normalized_qvec(2) << " ";
            line << normalized_qvec(3) << " ";

            // TVEC
            line << image.second.Tvec(0) << " ";
            line << image.second.Tvec(1) << " ";
            line << image.second.Tvec(2) << " ";

            line << image.second.CameraId() << " ";

            line << image.second.Name();

            file << line.str() << std::endl;

            line.str("");
            line.clear();

            for (const Point2D& point2D : image.second.Points2D()) {
            line << point2D.X() << " ";
            line << point2D.Y() << " ";
            if (point2D.HasPoint3D()) {
                line << point2D.Point3DId() << " ";
            } else {
                line << -1 << " ";
            }
            }
            line_string = line.str();
            line_string = line_string.substr(0, line_string.size() - 1);
            file << line_string << std::endl;
        }
    }

    void ColmapSFMDataIO::WritePoints3DText(const std::string& path) const {
        std::ofstream file(path, std::ios::trunc);

        // Ensure that we don't loose any precision by storing in text.
        file.precision(17);

        file << "# 3D point list with one line of data per point:" << std::endl;
        file << "#   POINT3D_ID, X, Y, Z, R, G, B, ERROR, "
                "TRACK[] as (IMAGE_ID, POINT2D_IDX)"
            << std::endl;
        file << "# Number of points: " << points3D_.size() << std::endl;

        for (const auto& point3D : points3D_) {
            file << point3D.first << " ";
            file << point3D.second.XYZ()(0) << " ";
            file << point3D.second.XYZ()(1) << " ";
            file << point3D.second.XYZ()(2) << " ";
            file << static_cast<int>(point3D.second.Color(0)) << " ";
            file << static_cast<int>(point3D.second.Color(1)) << " ";
            file << static_cast<int>(point3D.second.Color(2)) << " ";
            file << point3D.second.Error() << " ";

            std::ostringstream line;
            line.precision(17);

            for (const auto& track_el : point3D.second.Track().Elements()) {
                line << track_el.image_id << " ";
                line << track_el.point2D_idx << " ";
            }

            std::string line_string = line.str();
            line_string = line_string.substr(0, line_string.size() - 1);

            file << line_string << std::endl;
        }
    }
}