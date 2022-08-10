#include "quickstartpanel.hpp"

typedef std::function<void(std::vector<AlicePipelineFlows>)> AliceExecuteFlows;

std::vector<std::string> flowNameList {
  "当前无任何操作.",
  "相机初始化开始...",
  "相机初始化完成, 特征提取中...",
  "特征提取完成, 图像匹配中...",
  "图像匹配完成, 特征匹配中...",
  "特征匹配完成, SFM重建中...",
  "SFM重建完成.",
  "SFM重建完成, 场景准备中...",
  "场景准备完成, 深度图估计中...",
  "深度图估计完成, 深度图优化中...",
  "深度图优化完成, Meshing...",
  "Mesh完成, Mesh优化中...",
  "Mesh优化, 输出最终Textured Mesh."
};

QuickStartPanel::QuickStartPanel() : isStartSFMFlow_(false), isStartMVSFlow_(false), isWorking_(false), patten_(0) {
    isExistPhoto_ = false;
}

QuickStartPanel::~QuickStartPanel() {}

void QuickStartPanel::OnImGui(int width, int height) {
    ImGuiWindowFlags flags{ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize};
    ImGui::Begin("快速开始区", nullptr, flags);
    ImGui::SetWindowPos(ImVec2(width, 24), ImGuiCond_Always);
    ImGui::SetWindowSize(ImVec2(width*2 , height/5));

    ImGui::BeginDisabled(!isExistPhoto_);
    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0, 100, 255, 255));
    if (ImGui::Button("初始化", ImVec2(100, 50))) {
        if (!isStartSFMFlow_ && !isStartMVSFlow_) pipeline_.ReSetPipelineFlow();
    }

    ImGui::SameLine();
    if (ImGui::Button("点云生成", ImVec2(100, 50))) {
        if (pipeline_.GetPipelineFlow() == 0) {
            isStartSFMFlow_ = true;
            isStartMVSFlow_ = false;
            isUploaded_ = false;
        } 
    }
    ImGui::SameLine();
    if (ImGui::Button("模型拓扑", ImVec2(100, 50))) {
        if (pipeline_.GetPipelineFlow() == 6) {
            patten_ = 2;
            isStartSFMFlow_ = false;
            isStartMVSFlow_ = true;
            isUploaded_ = false;
        }
    }
    ImGui::EndDisabled();

    ImGui::PopStyleColor();

    ImGui::ProgressBar(pipeline_.GetPipelineFlow() / 12.0f, ImVec2(width*2 - 24, 0.0f));
    ImGui::Text("当前重建步骤>>> %s", flowNameList[pipeline_.GetPipelineFlow()].c_str());

    ImGui::End();
}

void QuickStartPanel::LoadPipeLineConfig(std::string exePath, std::string projectPath,
                    std::vector<std::string> photosPath, std::string density, int seqLen)
{
    pipeline_.SetEXEPath(exePath);
    pipeline_.SetWorkspacePath(projectPath);
    pipeline_.SetImagesPath(photosPath);
    pipeline_.SetDensity(density);
    pipeline_.SetSeqLength(seqLen);
}

int QuickStartPanel::ExecPipeLine() {
    std::vector<AlicePipelineFlows> flowsList;

    if ((isStartSFMFlow_ && !isWorking_) || (isStartMVSFlow_ && !isWorking_)) {
        isWorking_ = true;
        if (patten_ == 0) flowsList = {1, 2, 3, 5, 6};
        if (patten_ == 1) flowsList = {1, 2, 4, 5, 6};
        if (patten_ == 2) flowsList = {7, 8, 9, 10, 11, 12};

        AliceExecuteFlows ExecuteFlows = std::bind(&AlicePipe::ExecuteCurrentFlow, &pipeline_, std::placeholders::_1);
        std::thread AliceThread(ExecuteFlows, flowsList);
        if (AliceThread.joinable()) AliceThread.detach();
    }

    if (!isStartSFMFlow_ && !isStartMVSFlow_) return -1;

    if (isStartSFMFlow_ && (pipeline_.GetPipelineFlow() == 6) && !isUploaded_) {
        ImGui::OpenPopup("SFMResultPopUpWindow");
        isWorking_ = false;
        isStartSFMFlow_ = false;
        return 1;
    } else if (isStartMVSFlow_ && (pipeline_.GetPipelineFlow() == 12) && !isUploaded_) {
        isWorking_ = false;
        isStartMVSFlow_ = false;
        return 2;
    }
    return 0;
}

void QuickStartPanel::SetExecPatten(int patten) {
    patten_ = patten;
}

void QuickStartPanel::StartMVS() {
    isStartMVSFlow_ = true;
    isUploaded_ = false;
}

void QuickStartPanel::SetIsExistPhoto(bool isExisted) {
    isExistPhoto_ = isExisted;
}

bool QuickStartPanel::GetIsUploaded() {
    return isUploaded_;
}

void QuickStartPanel::SetIsUploaded(bool isUploaded) {
    isUploaded_ = isUploaded;
}