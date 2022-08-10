#include "sfmDataIOFromMeshroom.hpp"

#include <iostream>
#include <fstream>
#include <vector>

namespace HoleMap
{
    void MeshroomSFMDataIO::loadSFMFromJSON(const std::string& path){
        // 载入meshroom的sfm文件
        std::ifstream fs(path);
        nlohmann::json sfmData;
        fs >> sfmData;
        // Camera等价于intrinsics
        std::unordered_map<std::string, uint32_t> intrIdStringMapIntrIdInt;
        const size_t num_cameras = sfmData["intrinsics"].size();
        for (size_t i = 0; i < num_cameras; ++i) {
            intrIdStringMapIntrIdInt.emplace(sfmData["intrinsics"][i]["intrinsicId"], i);

            class Camera camera;
            camera.SetCameraId(i);
            camera.SetModelId(1); // 默认PINHOLE
            camera.SetWidth(stoi(sfmData["intrinsics"][i]["width"].get<std::string>()));
            camera.SetHeight(stoi(sfmData["intrinsics"][i]["height"].get<std::string>()));
            camera.SetParams(
                stod(sfmData["intrinsics"][i]["pxFocalLength"].get<std::string>()),
                stod(sfmData["intrinsics"][i]["pxFocalLength"].get<std::string>()),
                stod(sfmData["intrinsics"][i]["principalPoint"][0].get<std::string>()),
                stod(sfmData["intrinsics"][i]["principalPoint"][1].get<std::string>()));
            cameras_.emplace(camera.CameraId(), camera);
        }
        // images等价于poses
        // 获取所有views
        const size_t num_views = sfmData["views"].size();
        std::unordered_map<std::string, uint32_t> viewIdStringMapViewIdInt;
        std::unordered_map<std::string, std::string> viewIdStringMapImageName;
        std::unordered_map<std::string, uint32_t> viewIdStringMapIntrIdInt;
        for (size_t i = 0; i < num_views; ++i) {
            viewIdStringMapViewIdInt.emplace(sfmData["views"][i]["viewId"], i);
        
            viewIdStringMapIntrIdInt.emplace(
                sfmData["views"][i]["viewId"],
                intrIdStringMapIntrIdInt[sfmData["views"][i]["intrinsicId"]]);
       
            std::filesystem::path imgPath = sfmData["views"][i]["path"];
            viewIdStringMapImageName.emplace(
                sfmData["views"][i]["viewId"], imgPath.filename().c_str());
        }
        // 创建poses
        std::vector<std::string> viewIds;
        for (const auto& viewId : viewIdStringMapViewIdInt) {
            viewIds.push_back(viewId.first);
        }
        for (auto pose : sfmData["poses"]) {
            auto findRes = std::find(
                viewIds.begin(), viewIds.end(), pose["poseId"].get<std::string>());

            if (findRes == viewIds.end()) {
                std::cout << "sfm.json已损坏!" << std::endl;
                return;
            }
            class Image image;
            image.SetImageId(viewIdStringMapViewIdInt[pose["poseId"]]);
            image.SetCameraId(viewIdStringMapIntrIdInt[pose["poseId"]]);
            image.SetName(viewIdStringMapImageName[pose["poseId"]]);
            int rawTmp = 0, colTmp = 0;
            Eigen::Matrix3d rotateTmp;
            for (auto rt : pose["pose"]["transform"]["rotation"]) {
                int rawIdx = rawTmp / 3;
                if (colTmp == 3) colTmp = 0;
                // std::cout << rawIdx << " " << colTmp << std::endl;
                rotateTmp(rawIdx, colTmp) = stod(rt.get<std::string>());
                colTmp++, rawTmp++;
            }
            image.SetRotate(rotateTmp);
            image.Rotate2Qvec();
            image.NormalizeQvec();

            Eigen::Vector3d centerTmp;
            int otherTmp = 0;
            for (auto ct : pose["pose"]["transform"]["center"]) {
                // std::cout << otherTmp << std::endl;
                centerTmp(otherTmp) = stod(ct.get<std::string>());
                otherTmp++;
            }
            image.SetCenter(centerTmp);
            image.Center2Tvec();

            images_.emplace(image.ImageId(), image);
        }
        // 创建point3D
        const size_t num_points3D = sfmData["structure"].size();
        for (size_t i = 0; i < num_points3D; ++i) {
            class Point3D point3D;

            const uint64_t point3D_id = stoi(sfmData["structure"][i]["landmarkId"].get<std::string>());

            point3D.XYZ()(0) = stod(sfmData["structure"][i]["X"][0].get<std::string>());
            point3D.XYZ()(1) = stod(sfmData["structure"][i]["X"][1].get<std::string>());
            point3D.XYZ()(2) = stod(sfmData["structure"][i]["X"][2].get<std::string>());
            point3D.Color(0) = stoi(sfmData["structure"][i]["color"][0].get<std::string>());
            point3D.Color(1) = stoi(sfmData["structure"][i]["color"][1].get<std::string>());
            point3D.Color(2) = stoi(sfmData["structure"][i]["color"][2].get<std::string>());

            const size_t track_length = sfmData["structure"][i]["observations"].size();
            for (size_t j = 0; j < track_length; ++j) {
                const uint32_t image_id = viewIdStringMapViewIdInt[sfmData["structure"][i]["observations"][j]["observationId"]];
                class Point2D point2D;
                Eigen::Vector2d xyTemp{
                    stod(sfmData["structure"][i]["observations"][j]["x"][0].get<std::string>()),
                    stod(sfmData["structure"][i]["observations"][j]["x"][1].get<std::string>())};
                point2D.SetXY(xyTemp);
                point2D.SetPoint3DId(point3D_id);
                images_[image_id].AddPoint2D(point2D);
                // 编号: 图片 特征点
                const uint32_t point2D_idx = images_[image_id].NumPoints2D() - 1;
                // std::cout << "image_id: " << image_id << " , point2d_id: " << point2D_idx << std::endl;
                point3D.Track().AddElement(image_id, point2D_idx);
            }
            point3D.Track().Compress();
            points3D_.emplace(point3D_id, point3D);
        }
        // Set the point as triangulated
        for (const auto& image : images_) {
            uint32_t num_points3D = 0;
            for (uint32_t point2D_idx = 0; point2D_idx < image.second.NumPoints2D(); ++point2D_idx) {
                class Point2D point2DTmp = image.second.Point2D(point2D_idx);
                if (point2DTmp.Point3DId() != kInvalidPoint3DId) {
                    num_points3D++;
                }
            }
            images_[image.second.ImageId()].SetNumPoints3D(num_points3D);
        }
    }

    // 图片路径需要指明
    void MeshroomSFMDataIO::writeSFMIntoJSON(const std::string& imagesPath, const std::string& outPath){
        nlohmann::json outSFMData;
        outSFMData["version"] = {"1", "0", "0"};
        for (const auto& image : images_) {
            nlohmann::json imageDataTmp;
            nlohmann::json metaDataTmp;
            imageDataTmp["viewId"] = std::to_string(image.second.ImageId());
            imageDataTmp["poseId"] = std::to_string(image.second.ImageId());
            imageDataTmp["intrinsicId"] = std::to_string(image.second.CameraId());
            imageDataTmp["resectionId"] = std::to_string(image.second.ImageId());
            imageDataTmp["path"] = joinPaths(imagesPath, image.second.Name());
            imageDataTmp["width"] = std::to_string(cameras_[image.second.CameraId()].Width());
            imageDataTmp["height"] = std::to_string(cameras_[image.second.CameraId()].Height());
            metaDataTmp["oiio:ColorSpace"] = "sRGB";
            imageDataTmp["metadata"] = metaDataTmp;
            outSFMData["views"].push_back(imageDataTmp);
            nlohmann::json poseDataTmp;
            poseDataTmp["poseId"] = std::to_string(image.second.ImageId());
            poseDataTmp["pose"]["transform"]["rotation"] = {
                std::to_string(image.second.Rotate(0, 0)),
                std::to_string(image.second.Rotate(0, 1)),
                std::to_string(image.second.Rotate(0, 2)),
                std::to_string(image.second.Rotate(1, 0)),
                std::to_string(image.second.Rotate(1, 1)),
                std::to_string(image.second.Rotate(1, 2)),
                std::to_string(image.second.Rotate(2, 0)),
                std::to_string(image.second.Rotate(2, 1)),
                std::to_string(image.second.Rotate(2, 2))
            };
            poseDataTmp["pose"]["transform"]["center"] = {
                std::to_string(image.second.Center(0)),
                std::to_string(image.second.Center(1)),
                std::to_string(image.second.Center(2))
            };
            poseDataTmp["pose"]["locked"] = "0";
            outSFMData["poses"].push_back(poseDataTmp);
        }
        for (const auto& camera : cameras_) {
            nlohmann::json cameraDataTmp;
            cameraDataTmp["intrinsicId"] = std::to_string(camera.second.CameraId());
            cameraDataTmp["width"] = std::to_string(camera.second.Width());
            cameraDataTmp["height"] = std::to_string(camera.second.Height());
            cameraDataTmp["serialNumber"] = "";
            cameraDataTmp["type"] = "pinhole";
            cameraDataTmp["initializationMode"] = "estimated";

            if (camera.second.ModelId() == 0 || camera.second.ModelId() == 2) {
                cameraDataTmp["pxInitialFocalLength"] = std::to_string(camera.second.Params(0));
                cameraDataTmp["pxFocalLength"] = std::to_string(camera.second.Params(0));
                cameraDataTmp["principalPoint"] = {
                    std::to_string(camera.second.Params(1)),
                    std::to_string(camera.second.Params(2)),
                };
            } else {
                cameraDataTmp["pxInitialFocalLength"] = std::to_string(camera.second.Params(0));
                cameraDataTmp["pxFocalLength"] = std::to_string(camera.second.Params(1));
                cameraDataTmp["principalPoint"] = {
                    std::to_string(camera.second.Params(2)),
                    std::to_string(camera.second.Params(3)),
                };
            }
            
            cameraDataTmp["distortionParams"] = {"0", "0", "0"};
            cameraDataTmp["locked"] = "0";
            outSFMData["intrinsics"].push_back(cameraDataTmp);
        }
        for (const auto& point3d : points3D_) {
            nlohmann::json point3dDataTmp;
            class Track tracker = point3d.second.Track();
            size_t num_points2d = tracker.Length();
            if (num_points2d <= 0) continue;
            struct TrackElement trackElem0 = tracker.Element(0);
            point3dDataTmp["landmarkId"] = std::to_string(
                images_[trackElem0.image_id].Point2D(trackElem0.point2D_idx).Point3DId());
            point3dDataTmp["descType"] = "sift";
            point3dDataTmp["color"] = {
                std::to_string(point3d.second.Color(0)),
                std::to_string(point3d.second.Color(1)),
                std::to_string(point3d.second.Color(2))};
            point3dDataTmp["X"] = {
                std::to_string(point3d.second.XYZ(0)),
                std::to_string(point3d.second.XYZ(1)),
                std::to_string(point3d.second.XYZ(2))};

            for (auto trackElem : tracker.Elements()) {
                nlohmann::json point2dDataTmp;
                point2dDataTmp["observationId"] = std::to_string(trackElem.image_id);
                point2dDataTmp["featureId"] = std::to_string(trackElem.point2D_idx);
                point2dDataTmp["x"] = {
                    std::to_string(images_[trackElem.image_id].Point2D(trackElem.point2D_idx).X()),
                    std::to_string(images_[trackElem.image_id].Point2D(trackElem.point2D_idx).Y())};
                point3dDataTmp["observations"].push_back(point2dDataTmp);
            }
            outSFMData["structure"].push_back(point3dDataTmp);
        }
        std::ofstream o(joinPaths(outPath, "sfm.json"));
        o << std::setw(4) << outSFMData <<std::endl;
    }
}