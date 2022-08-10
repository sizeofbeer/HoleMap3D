#pragma once
#include "abcg.hpp"
#include "misc.hpp"
#include <sstream>

using AlicePipelineFlows = int;

enum AlicePipelineFlows_{
    AlicePipelineFlows_None = 0,
    AlicePipelineFlows_CameraInit = 1,
    AlicePipelineFlows_FeatureExtract = 2,
    AlicePipelineFlows_ImageMatchVocabularyTree = 3,
    AlicePipelineFlows_ImageMatchSequential = 4,
    AlicePipelineFlows_FeatureMatch = 5,
    AlicePipelineFlows_IncrementalSfM = 6,
    AlicePipelineFlows_PrepareDenseScene = 7,
    AlicePipelineFlows_DepthMap = 8,
    AlicePipelineFlows_DepthMapFilter = 9,
    AlicePipelineFlows_Meshing = 10,
    AlicePipelineFlows_MeshFilter = 11,
    AlicePipelineFlows_Texturing = 12,
};

namespace HoleMap {
    class AlicePipeline{
    public:
        AlicePipeline();
        ~AlicePipeline();
        inline bool SetPipelineFlow(AlicePipelineFlows flows);
        inline void ReSetPipelineFlow();
        inline AlicePipelineFlows GetPipelineFlow();
        void ExecuteCurrentFlow(std::vector<AlicePipelineFlows> flowsList);
        inline void SetEXEPath(std::string EXEPath);
        inline void SetWorkspacePath(std::string workspacePath);
        inline void SetImagesPath(std::vector<std::string> &imagesPath);
        inline void SetDensity(std::string density);
        inline void SetSeqLength(int seqLen);

    private:
        void FindCurrentFlowCommand(AlicePipelineFlows flows, std::vector<std::string> &aliceCommands);

        AlicePipelineFlows next_flows_;
        std::string aliceEXEPath_;
        std::string workspacePath_;
        std::vector<std::string> imagesPath_;
        std::vector<std::string> aliceCommands_;
        std::string density_;
        int seqLength_;
        int finishedNum_;
        int finishedChinks_;
    };

    inline AlicePipelineFlows AlicePipeline::GetPipelineFlow() {
        return next_flows_;
    }
    inline void AlicePipeline::SetEXEPath(std::string EXEPath) {
        aliceEXEPath_ = EXEPath;
    }
    inline void AlicePipeline::SetWorkspacePath(std::string workspacePath) {
        workspacePath_ = workspacePath;
    }
    inline void AlicePipeline::SetImagesPath(std::vector<std::string> &imagesPath) {
        imagesPath_.clear();
        for (auto imagePath : imagesPath) {
            imagesPath_.push_back(imagePath);
        }
    }
    inline void AlicePipeline::ReSetPipelineFlow() {
        next_flows_ = AlicePipelineFlows_None;
    }
    inline bool AlicePipeline::SetPipelineFlow(AlicePipelineFlows flows) {
        if (flows == AlicePipelineFlows_ImageMatchSequential && next_flows_ == AlicePipelineFlows_FeatureExtract) {
            return true;
        } else if (flows == AlicePipelineFlows_FeatureMatch && next_flows_ == AlicePipelineFlows_ImageMatchVocabularyTree) {
            return true;
        } else {
            if (flows == AlicePipelineFlows_ImageMatchSequential && next_flows_ == AlicePipelineFlows_ImageMatchVocabularyTree) {
                return false;
            } else if (flows == (next_flows_+1)) {
                return true;
            } else {
                return false;
            }
        }
        return true;
    }
    inline void AlicePipeline::SetDensity(std::string density) {
        density_ = density;
    }
    inline void AlicePipeline::SetSeqLength(int seqLen) {
        seqLength_ = seqLen * 1.375;
    }
}