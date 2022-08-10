#pragma once
#include "abcg.hpp"

#include "aliceVisionPipeline.h"

#include <thread>
#include <filesystem>

using AlicePipe = HoleMap::AlicePipeline;

class QuickStartPanel {
public:
    QuickStartPanel();
    ~QuickStartPanel();

    void LoadPipeLineConfig(std::string exePath, std::string projectPath,
                    std::vector<std::string> photosPath, std::string density = "normal", int seqLen = 24);
    void OnImGui(int width, int height);
    int ExecPipeLine();
    void SetExecPatten(int patten);
    void StartMVS();
    void SetIsExistPhoto(bool isExisted);
    bool GetIsUploaded();
    void SetIsUploaded(bool isUploaded);

private:
    bool isStartSFMFlow_;
    bool isStartMVSFlow_;
    bool isWorking_;
    bool isUploaded_;
    bool isExistPhoto_;
    int patten_;
    AlicePipe pipeline_;
};
