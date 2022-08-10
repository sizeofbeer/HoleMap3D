#pragma once
#include "camera.h"
#include "image.h"
#include "point3d.h"

#include <string>

namespace HoleMap
{
    class ColmapSFMDataIO
    {
    public:
        void ReadCamerasBinary(const std::string& path);
        void ReadImagesBinary(const std::string& path);
        void ReadPoints3DBinary(const std::string& path);

        void WriteCamerasText(const std::string& path) const;
        void WriteImagesText(const std::string& path) const;
        void WritePoints3DText(const std::string& path) const;

        // Get mutable objects.
        inline class Camera& Camera(const uint32_t camera_id);
        inline class Image& Image(const uint32_t image_id);
        inline class Point3D& Point3D(const uint64_t point3D_id);

        inline std::unordered_map<uint32_t, class Camera>& Cameras();
        inline std::unordered_map<uint32_t, class Image>& Images();
        inline std::unordered_map<uint64_t, class Point3D>& Points3D();

        inline void CamerasClone(std::unordered_map<uint32_t, class Camera>& cameras);
        inline void ImagesClone(std::unordered_map<uint32_t, class Image>& images);
        inline void Points3DClone(std::unordered_map<uint64_t, class Point3D>& points3D);

    private:
        std::unordered_map<uint32_t, class Camera> cameras_;
        std::unordered_map<uint32_t, class Image> images_;
        std::unordered_map<uint64_t, class Point3D> points3D_;
    };

    class Camera& ColmapSFMDataIO::Camera(const uint32_t camera_id) {
        return cameras_.at(camera_id);
    }
    class Image& ColmapSFMDataIO::Image(const uint32_t image_id) {
        return images_.at(image_id);
    }
    class Point3D& ColmapSFMDataIO::Point3D(const uint64_t point3D_id) {
        return points3D_.at(point3D_id);
    }
    std::unordered_map<uint32_t, class Camera>& ColmapSFMDataIO::Cameras() {
        return cameras_;
    }
    std::unordered_map<uint32_t, class Image>& ColmapSFMDataIO::Images() {
        return images_;
    }
    std::unordered_map<uint64_t, class Point3D>& ColmapSFMDataIO::Points3D() {
        return points3D_;
    }
    void ColmapSFMDataIO::CamerasClone(std::unordered_map<uint32_t, class Camera>& cameras) {
        cameras_ = cameras;
    }
    void ColmapSFMDataIO::ImagesClone(std::unordered_map<uint32_t, class Image>& images){
        images_ = images;
    }
    void ColmapSFMDataIO::Points3DClone(std::unordered_map<uint64_t, class Point3D>& points3D){
        points3D_ = points3D;
    }
}