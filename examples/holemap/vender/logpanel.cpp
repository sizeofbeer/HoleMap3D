#include "logpanel.hpp"

LogPanel::LogPanel() {}

LogPanel::~LogPanel() {}

void LogPanel::OnImGui(int width, int height, int x, int y) {
    auto flags{ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove};
    ImGui::Begin("日志区", nullptr, flags);
    ImGui::SetWindowPos(ImVec2(x, y));
    ImGui::SetWindowSize(ImVec2(width, height));
    ImGui::Text("Context>>>");
    ImGui::Text("%s", logInfo_.c_str());
    ImGui::End();
}