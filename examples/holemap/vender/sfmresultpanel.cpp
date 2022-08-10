#include "sfmresultpanel.hpp"


SFMResultPanel::SFMResultPanel() : photo_total_nums_(0), photo_sfmFailed_nums_(0) {}

SFMResultPanel::~SFMResultPanel() {}

void SFMResultPanel::OnImGui(int width, int height)
{
    // Always center this window when appearing
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    if (ImGui::BeginPopupModal("SFMResultPopUpWindow", NULL, ImGuiWindowFlags_NoResize)) {
        ImGui::SetWindowPos(ImVec2(center[0] - width/2, center[1] - height/2));
        ImGui::SetWindowSize(ImVec2(width, height));

        ImGui::Text("点云生成成功照片数: %d, 失败照片数: %d",
                                photo_total_nums_ - photo_sfmFailed_nums_, photo_sfmFailed_nums_);
        ImGui::Text("是否继续>>>");
        if (ImGui::Button("是", ImVec2(120, 0)))
        {
            is_start_mvs_ = true;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("否", ImVec2(120, 0)))
        {
            is_start_mvs_ = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void SFMResultPanel::SetPhotoTotalNums(int nums) {
    photo_total_nums_ = nums;
}

void SFMResultPanel::SetPhotoSFMFailedNums(int nums) {
    photo_sfmFailed_nums_ = nums;
}