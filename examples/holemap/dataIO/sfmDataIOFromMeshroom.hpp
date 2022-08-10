#pragma once
#include "misc.hpp"
#include "camera.h"
#include "image.h"
#include "point3d.h"
#include "json.hpp"

#include <string>

namespace HoleMap
{
    class MeshroomSFMDataIO
    {
    public:
        void loadSFMFromJSON(const std::string& path);
        void writeSFMIntoJSON(const std::string& imagesPath, const std::string& outPath);
          
        // Get mutable objects.
        inline class Camera& Camera(const uint32_t camera_id);
        inline std::unordered_map<uint32_t, class Camera>& Cameras();
        inline std::unordered_map<uint32_t, class Image>& Images();
        inline std::unordered_map<uint64_t, class Point3D>& Points3D();
        inline class Image& Image(const uint32_t image_id);
        inline class Point3D& Point3D(const uint64_t point3D_id);

        inline void CamerasClone(std::unordered_map<uint32_t, class Camera>& cameras);
        inline void ImagesClone(std::unordered_map<uint32_t, class Image>& images);
        inline void Points3DClone(std::unordered_map<uint64_t, class Point3D>& points3D);

    private:
        std::unordered_map<uint32_t, class Camera> cameras_;
        std::unordered_map<uint32_t, class Image> images_;
        std::unordered_map<uint64_t, class Point3D> points3D_;
        std::vector<std::string> disrstImages_;
    };

    void MeshroomSFMDataIO::CamerasClone(std::unordered_map<uint32_t, class Camera>& cameras) {
        cameras_ = cameras;
    }
    void MeshroomSFMDataIO::ImagesClone(std::unordered_map<uint32_t, class Image>& images){
        images_ = images;
    }
    void MeshroomSFMDataIO::Points3DClone(std::unordered_map<uint64_t, class Point3D>& points3D){
        points3D_ = points3D;
    }

    std::unordered_map<uint32_t, class Camera>& MeshroomSFMDataIO::Cameras() {
        return cameras_;
    }
    std::unordered_map<uint32_t, class Image>& MeshroomSFMDataIO::Images() {
        return images_;
    }
    std::unordered_map<uint64_t, class Point3D>& MeshroomSFMDataIO::Points3D() {
        return points3D_;
    }

    class Camera& MeshroomSFMDataIO::Camera(const uint32_t camera_id) {
        return cameras_.at(camera_id);
    }

    class Image& MeshroomSFMDataIO::Image(const uint32_t image_id) {
        return images_.at(image_id);
    }
    
    class Point3D& MeshroomSFMDataIO::Point3D(const uint64_t point3D_id) {
        return points3D_.at(point3D_id);
    }
}