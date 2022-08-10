#include "attributepanel.hpp"

static const char* matchItems[] = {"VocabularyTree", "Sequential"};
static const char* viewItems[] = {"坐标轴", "稀疏点云", "网格模型"};

static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

AttributePanel::AttributePanel() {
    featureExtractLevel_ = 0;
    seqLength_ = 24;
    imageMatchType_ = 0;
    viewObjectType_ = 0;
}

AttributePanel::~AttributePanel() {}

void AttributePanel::OnImGui(int width, int height, int x, int y) {
    auto flags{ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize};
    ImGui::Begin("属性区", nullptr, flags);
    ImGui::SetWindowPos(ImVec2(x, y));
    ImGui::SetWindowSize(ImVec2(width, height));

    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
    {
        if (ImGui::BeginTabItem("可选性"))
        {
            ImGui::Text("特征提取设置");
            ImGui::Combo("提取级别", &featureExtractLevel_, extractItems, IM_ARRAYSIZE(extractItems));
            ImGui::SameLine(); HelpMarker(
                    "normal:    SIFT特征数量<= 20000\n"
                    "high:      SIFT特征数量<= 50000");

            ImGui::Text("图像匹配设置");
            ImGui::Combo("匹配方法", &imageMatchType_, matchItems, IM_ARRAYSIZE(matchItems));
            ImGui::SameLine(); HelpMarker(
                    "VocabularyTree: 预训练模型, 通过图像特征来计算图像相似度\n"
                    "Sequential:     序列帧模式, 适用于连续的、变化不剧烈的一组照片(环拍)");

            if (imageMatchType_ == 1) {
                ImGui::DragInt("序列数", &seqLength_, 1);
                ImGui::SameLine(); HelpMarker(
                    "环绕物体一圈的拍摄数, 默认24.");
            }

            ImGui::Text("三维预览设置");
            ImGui::Combo("预览对象", &viewObjectType_, viewItems, IM_ARRAYSIZE(viewItems));
            ImGui::SameLine(); HelpMarker(
                    "稀疏点云: SFM重建后得到物体的一些稀疏的点和相机位姿信息\n"
                    "网格模型: Mesh重建后得到物体的表面信息(点/面/纹理)");

            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();
}