#include "aliceVisionPipeline.h"

#include "threadpool.h"
#include <chrono>
#include <math.h>
#include <thread>

namespace HoleMap {

    AlicePipeline::AlicePipeline(){
        next_flows_ = AlicePipelineFlows_None;
    }

    AlicePipeline::~AlicePipeline(){}

    void AlicePipeline::FindCurrentFlowCommand(AlicePipelineFlows flows, std::vector<std::string> &aliceCommands) {
        aliceCommands.clear();
        if (flows == AlicePipelineFlows_CameraInit) {
            std::filesystem::path outputPath = joinPaths(workspacePath_, "CameraInit");
            if (!std::filesystem::exists(outputPath)) std::filesystem::create_directory(outputPath);

            std::stringstream aliceCommand;
            aliceCommand << aliceEXEPath_.c_str() << "/bin/aliceVision_cameraInit "
                << "--sensorDatabase '" << aliceEXEPath_.c_str() << "/share/aliceVision/cameraSensors.db' "
                << "--defaultFieldOfView 45.0 --groupCameraFallback folder "
                << "--allowedCameraModels pinhole,radial1,radial3,brown,fisheye4,fisheye1 "
                << "--useInternalWhiteBalance True --viewIdMethod metadata --verboseLevel info "
                << "--output '" << workspacePath_.c_str() << "/CameraInit/cameraInit.sfm' --allowSingleView 1 "
                << "--imageFolder '" << workspacePath_.c_str() << "/symlinks'";
            aliceCommands.push_back(aliceCommand.str());
        } else if (flows == AlicePipelineFlows_FeatureExtract) {
            std::filesystem::path outputPath = joinPaths(workspacePath_, "FeatureExtraction");
            if (!std::filesystem::exists(outputPath)) std::filesystem::create_directory(outputPath);
            
            int chunks = ceil(imagesPath_.size() / 80.0f);
            for (int i = 0; i < chunks; i++) {
                std::stringstream aliceCommand;
                aliceCommand << aliceEXEPath_.c_str() << "/bin/aliceVision_featureExtraction "
                    << "--input '" <<  workspacePath_.c_str() << "/CameraInit/cameraInit.sfm' --describerTypes sift "
                    << "--describerPreset " << density_ << " --describerQuality " << density_ << " --contrastFiltering GridSort "
                    << "--gridFiltering True --forceCpuExtraction False --maxThreads 0 "
                    << "--verboseLevel info --output '" <<  workspacePath_.c_str() << "/FeatureExtraction' "
                    << "--rangeStart " << i * 80 << " --rangeSize 80";
                aliceCommands.push_back(aliceCommand.str());
            }
        } else if (flows == AlicePipelineFlows_ImageMatchVocabularyTree) {
            std::filesystem::path outputPath = joinPaths(workspacePath_, "ImageMatching");
            if (!std::filesystem::exists(outputPath)) std::filesystem::create_directory(outputPath);
            
            std::stringstream aliceCommand;
            aliceCommand << aliceEXEPath_.c_str() << "/bin/aliceVision_imageMatching "
                << "--input '" << workspacePath_.c_str() << "/CameraInit/cameraInit.sfm' "
                << "--featuresFolders '" << workspacePath_.c_str() << "/FeatureExtraction' "
                << "--method VocabularyTree --tree '" << aliceEXEPath_.c_str() << "/share/aliceVision/vlfeat_K80L3.SIFT.tree' "
                << "--weights '' --minNbImages 200 --maxDescriptors 500 --nbMatches 50 --verboseLevel info "
                << "--output '" << workspacePath_.c_str() << "/ImageMatching/imageMatches.txt'";
            aliceCommands.push_back(aliceCommand.str());
        } else if (flows == AlicePipelineFlows_ImageMatchSequential) {
            std::filesystem::path outputPath = joinPaths(workspacePath_, "ImageMatching");
            if (!std::filesystem::exists(outputPath)) std::filesystem::create_directory(outputPath);

            std::stringstream aliceCommand;
            aliceCommand << aliceEXEPath_.c_str() << "/bin/aliceVision_imageMatching "
                << "--input '" << workspacePath_.c_str() << "/CameraInit/cameraInit.sfm' "
                << "--featuresFolders '" << workspacePath_.c_str() << "/FeatureExtraction' "
                << "--method Sequential --nbNeighbors "<< seqLength_ << " --verboseLevel info "
                << "--output '" << workspacePath_.c_str() << "/ImageMatching/imageMatches.txt'";
            aliceCommands.push_back(aliceCommand.str());
        } else if (flows == AlicePipelineFlows_FeatureMatch) {
            std::filesystem::path outputPath = joinPaths(workspacePath_, "FeatureMatching");
            if (!std::filesystem::exists(outputPath)) std::filesystem::create_directory(outputPath);
            
            int chunks = ceil(imagesPath_.size() / 40.0f);
            for (int i = 0; i < chunks; i++) {
                std::stringstream aliceCommand;
                aliceCommand << aliceEXEPath_.c_str() << "/bin/aliceVision_featureMatching "
                    << "--input '" << workspacePath_.c_str() << "/CameraInit/cameraInit.sfm' "
                    << "--featuresFolders '" << workspacePath_.c_str() << "/FeatureExtraction' "
                    << "--imagePairsList '" << workspacePath_.c_str() << "/ImageMatching/imageMatches.txt' "
                    << "--describerTypes sift --photometricMatchingMethod ANN_L2 --geometricEstimator acransac "
                    << "--geometricFilterType fundamental_matrix --distanceRatio 0.8 --maxIteration 2048 "
                    << "--geometricError 0.0 --knownPosesGeometricErrorMax 5.0 --maxMatches 0 "
                    << "--savePutativeMatches False --crossMatching False --guidedMatching False "
                    << "--matchFromKnownCameraPoses False --exportDebugFiles False --verboseLevel info "
                    << "--output '" << workspacePath_.c_str() << "/FeatureMatching' "
                    << "--rangeStart " << i * 40 << " --rangeSize 40";
                aliceCommands.push_back(aliceCommand.str());
            }
        } else if (flows == AlicePipelineFlows_IncrementalSfM) {
            std::filesystem::path outputPath = joinPaths(workspacePath_, "StructureFromMotion");
            if (!std::filesystem::exists(outputPath)) std::filesystem::create_directory(outputPath);

            std::stringstream aliceCommand;
            aliceCommand << aliceEXEPath_.c_str() << "/bin/aliceVision_incrementalSfM "
                << "--input '" << workspacePath_.c_str() << "/CameraInit/cameraInit.sfm' "
                << "--featuresFolders '" << workspacePath_.c_str() << "/FeatureExtraction' "
                << "--matchesFolders '" << workspacePath_.c_str() << "/FeatureMatching' "
                << "--describerTypes sift --localizerEstimator acransac --observationConstraint Basic "
                << "--localizerEstimatorMaxIterations 4096 --localizerEstimatorError 0.0 "
                << "--lockScenePreviouslyReconstructed False --useLocalBA True --localBAGraphDistance 1 "
                << "--maxNumberOfMatches 0 --minNumberOfMatches 0 --minInputTrackLength 2 "
                << "--minNumberOfObservationsForTriangulation 2 --minAngleForTriangulation 3.0 "
                << "--minAngleForLandmark 2.0 --maxReprojectionError 4.0 --minAngleInitialPair 5.0 "
                << "--maxAngleInitialPair 40.0 --useOnlyMatchesFromInputFolder False --useRigConstraint True "
                << "--lockAllIntrinsics False --filterTrackForks False --initialPairA '' --initialPairB '' "
                << "--interFileExtension .json --verboseLevel info "
                << "--output '" << workspacePath_.c_str() << "/StructureFromMotion/sfm.json' "
                << "--outputViewsAndPoses '" << workspacePath_.c_str() << "/StructureFromMotion/cameras.sfm' "
                << "--extraInfoFolder '" << workspacePath_.c_str() << "/StructureFromMotion'";
            aliceCommands.push_back(aliceCommand.str());
        } else if (flows == AlicePipelineFlows_PrepareDenseScene) {
            std::filesystem::path outputPath = joinPaths(workspacePath_, "PrepareDenseScene");
            if (!std::filesystem::exists(outputPath)) std::filesystem::create_directory(outputPath);

            int chunks = ceil(imagesPath_.size() / 80.0f);
            for (int i = 0; i < chunks; i++) {
                std::stringstream aliceCommand;
                aliceCommand << aliceEXEPath_.c_str() << "/bin/aliceVision_prepareDenseScene "
                    << "--input '" << workspacePath_.c_str() << "/StructureFromMotion/sfm.json' "
                    << "--outputFileType exr --saveMetadata True --saveMatricesTxtFiles False "
                    << "--evCorrection False --verboseLevel info "
                    << "--output '" <<workspacePath_.c_str() << "/PrepareDenseScene' "
                    << "--rangeStart " << i * 80 << " --rangeSize 80";
                aliceCommands.push_back(aliceCommand.str());
            }
        } else if (flows == AlicePipelineFlows_DepthMap) {
            std::filesystem::path outputPath = joinPaths(workspacePath_, "DepthMap");
            if (!std::filesystem::exists(outputPath)) std::filesystem::create_directory(outputPath);

            int chunks = ceil(imagesPath_.size() / 10.0f);
            for (int i = 0; i < chunks; i++) {
                std::stringstream aliceCommand;
                aliceCommand << aliceEXEPath_.c_str() << "/bin/aliceVision_depthMapEstimation "
                    << "--input '" << workspacePath_.c_str() << "/StructureFromMotion/sfm.json' "
                    << "--imagesFolder '" << workspacePath_.c_str() << "/PrepareDenseScene' "
                    << "--downscale 2 --minViewAngle 2.0 --maxViewAngle 70.0 --sgmMaxTCams 10 "
                    << "--sgmWSH 4 --sgmGammaC 5.5 --sgmGammaP 8.0 --refineMaxTCams 6 "
                    << "--refineNSamplesHalf 150 --refineNDepthsToRefine 31 --refineNiters 100 "
                    << "--refineWSH 3 --refineSigma 15 --refineGammaC 15.5 --refineGammaP 8.0 "
                    << "--refineUseTcOrRcPixSize False --exportIntermediateResults False --nbGPUs 0 "
                    << "--verboseLevel info "
                    << "--output '" << workspacePath_.c_str() << "/DepthMap' "
                    << "--rangeStart " << i * 10 << " --rangeSize 10";
                aliceCommands.push_back(aliceCommand.str());
            }
        } else if (flows == AlicePipelineFlows_DepthMapFilter) {
            std::filesystem::path outputPath = joinPaths(workspacePath_, "DepthMapFilter");
            if (!std::filesystem::exists(outputPath)) std::filesystem::create_directory(outputPath);

            int chunks = ceil(imagesPath_.size() / 20.0f);
            for (int i = 0; i < chunks; i++) {
                std::stringstream aliceCommand;
                aliceCommand << aliceEXEPath_.c_str() << "/bin/aliceVision_depthMapFiltering "
                    << "--input '" << workspacePath_.c_str() << "/StructureFromMotion/sfm.json' "
                    << "--depthMapsFolder '" << workspacePath_.c_str() << "/DepthMap' "
                    << "--minViewAngle 2.0 --maxViewAngle 70.0 --nNearestCams 10 "
                    << "--minNumOfConsistentCams 3 --minNumOfConsistentCamsWithLowSimilarity 4 "
                    << "--pixSizeBall 0 --pixSizeBallWithLowSimilarity 0 --computeNormalMaps False "
                    << "--verboseLevel info "
                    << "--output '" << workspacePath_.c_str() << "/DepthMapFilter' "
                    << "--rangeStart " << i * 20 << " --rangeSize 20";
                aliceCommands.push_back(aliceCommand.str());
            }
        } else if (flows == AlicePipelineFlows_Meshing) {
            std::filesystem::path outputPath = joinPaths(workspacePath_, "Meshing");
            if (!std::filesystem::exists(outputPath)) std::filesystem::create_directory(outputPath);

            std::stringstream aliceCommand;
            aliceCommand << aliceEXEPath_.c_str() << "/bin/aliceVision_meshing "
                << "--input '" << workspacePath_.c_str() << "/StructureFromMotion/sfm.json' "
                << "--depthMapsFolder '" << workspacePath_.c_str() << "/DepthMapFilter' "
                << "--estimateSpaceFromSfM True --estimateSpaceMinObservations 3 "
                << "--estimateSpaceMinObservationAngle 10 --maxInputPoints 50000000 "
                << "--maxPoints 5000000 --maxPointsPerVoxel 1000000 --minStep 2 "
                << "--partitioning singleBlock --repartition multiResolution "
                << "--angleFactor 15.0 --simFactor 15.0 --pixSizeMarginInitCoef 2.0 "
                << "--pixSizeMarginFinalCoef 4.0 --voteMarginFactor 4.0 "
                << "--contributeMarginFactor 2.0 --simGaussianSizeInit 10.0 "
                << "--simGaussianSize 10.0 --minAngleThreshold 1.0 --refineFuse True "
                << "--helperPointsGridSize 10 --nPixelSizeBehind 4.0 --fullWeight 1.0 "
                << "--voteFilteringForWeaklySupportedSurfaces True "
                << "--addLandmarksToTheDensePointCloud False "
                << "--invertTetrahedronBasedOnNeighborsNbIterations 10 "
                << "--minSolidAngleRatio 0.2 --nbSolidAngleFilteringIterations 2 "
                << "--colorizeOutput False --maxNbConnectedHelperPoints 50 "
                << "--saveRawDensePointCloud False --exportDebugTetrahedralization False --seed 0 "
                << "--verboseLevel info "
                << "--outputMesh '" << workspacePath_.c_str() << "/Meshing/mesh.obj' "
                << "--output '" << workspacePath_.c_str() << "/Meshing/densePointCloud.abc'";
            aliceCommands.push_back(aliceCommand.str());
        } else if (flows == AlicePipelineFlows_MeshFilter) {
            std::filesystem::path outputPath = joinPaths(workspacePath_, "MeshFiltering");
            if (!std::filesystem::exists(outputPath)) std::filesystem::create_directory(outputPath);

            std::stringstream aliceCommand;
            aliceCommand << aliceEXEPath_.c_str() << "/bin/aliceVision_meshFiltering "
                << "--inputMesh '" << workspacePath_.c_str() << "/Meshing/mesh.obj' "
                << "--keepLargestMeshOnly False --smoothingSubset all --smoothingBoundariesNeighbours 0 "
                << "--smoothingIterations 5 --smoothingLambda 1.0 --filteringSubset all "
                << "--filteringIterations 1 --filterLargeTrianglesFactor 60.0 "
                << "--filterTrianglesRatio 0.0 --verboseLevel info "
                << "--outputMesh '" << workspacePath_.c_str() << "/MeshFiltering/mesh.obj'";
            aliceCommands.push_back(aliceCommand.str());
        } else if (flows == AlicePipelineFlows_Texturing) {
            std::filesystem::path outputPath = joinPaths(workspacePath_, "Texturing");
            if (!std::filesystem::exists(outputPath)) std::filesystem::create_directory(outputPath);

            std::stringstream aliceCommand;
            aliceCommand << aliceEXEPath_.c_str() << "/bin/aliceVision_texturing "
                << "--input '" << workspacePath_.c_str() << "/Meshing/densePointCloud.abc' "
                << "--imagesFolder '" << workspacePath_.c_str() << "/PrepareDenseScene' "
                << "--inputMesh '" << workspacePath_.c_str() << "/MeshFiltering/mesh.obj' "
                << "--textureSide 8192 --downscale 2 --outputTextureFileType png "
                << "--unwrapMethod Basic --useUDIM True --fillHoles False --padding 5 "
                << "--multiBandDownscale 4 --multiBandNbContrib 1 5 10 0 --useScore True "
                << "--bestScoreThreshold 0.1 --angleHardThreshold 90.0 --processColorspace sRGB "
                << "--correctEV False --forceVisibleByAllVertices False --flipNormals False "
                << "--visibilityRemappingMethod PullPush --subdivisionTargetRatio 0.8 "
                << "--verboseLevel info "
                << "--output '" << workspacePath_.c_str() << "/Texturing'";
            aliceCommands.push_back(aliceCommand.str());
        }
    }

    void AlicePipeline::ExecuteCurrentFlow(std::vector<AlicePipelineFlows> flowsList)
    {      
        for (auto flows : flowsList) {
            bool is_executed = SetPipelineFlow(flows);
        
            if (is_executed) {
                FindCurrentFlowCommand(flows, aliceCommands_);

                for (auto aliceCommand : aliceCommands_)
                    system(aliceCommand.c_str());
                
                next_flows_ = flows;
            }
        }
    }
}