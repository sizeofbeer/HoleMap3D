#include "fileinfopanel.hpp"

FileInfoPanel::FileInfoPanel() : n_views_(0) {}

FileInfoPanel::~FileInfoPanel() {}

void FileInfoPanel::OnImGui(int width, int height, int x, int y) {
    auto flags{ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize};
    ImGui::Begin("文件信息区", nullptr, flags);
    ImGui::SetWindowPos(ImVec2(x, y));
    ImGui::SetWindowSize(ImVec2(width, height));
    ImGui::Text("图像数量: %d", n_views_);
    ImGui::Text("项目地址: ");
    ImGui::SameLine();
    ImGui::Text("%s", viewsPath_.c_str());
    ImGui::End();
}