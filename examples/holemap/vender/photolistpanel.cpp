#include "photolistpanel.hpp"

PhotoListPanel::PhotoListPanel() : isShowedView_(false), isShowedViewContext_(true) {}

PhotoListPanel::~PhotoListPanel() {}

void PhotoListPanel::OnImGui(int width, int height)
{
    ImGuiWindowFlags flags{ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize};
    ImGui::Begin("图片目录区", nullptr, flags);
    ImGui::SetWindowPos(ImVec2(0, 24), ImGuiCond_Always);
    ImGui::SetWindowSize(ImVec2(width, height), ImGuiCond_Always);

    int totalViews = allViewsNameList_->size();

    ImGui::Text("图片数量:%d", totalViews);
    if (ImGui::BeginTable("图片列表", 2, ImGuiTableFlags_ScrollY | ImGuiTableFlags_BordersOuter))
    {
        bool isOdd = (totalViews%2 == 0) ? false : true;
        int tableRows = isOdd ? (totalViews/2 + 1) : (totalViews/2);

        std::string currViewName;
        std::vector<std::string>::iterator iter;
        
        for (int i = 0; i < tableRows; i++) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            {
                currViewName = (allViewsMap_->at(allViewsNameList_->at(2*i)).filepath).filename();
                
                iter = find(failedViewsNameList_->begin(), failedViewsNameList_->end(), currViewName);
                if (iter != failedViewsNameList_->end()) {
                    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(255, 0, 0, 255));
                    ImGui::ImageButton((void *)(intptr_t)allViewsMap_->at(currViewName).texture,
                                        ImVec2(width / 2.5, width / 3.75));
                    ImGui::PopStyleColor();
                } else {
                    ImGui::ImageButton((void *)(intptr_t)allViewsMap_->at(currViewName).texture,
                                        ImVec2(width / 2.5, width / 3.75));
                }

                if (ImGui::BeginPopupContextItem()) {
                    if (ImGui::Selectable("删除")) {
                        std::string deteleCMD = "rm " + allViewsMap_->at(currViewName).filepath.string();
                        system(deteleCMD.c_str());
                        allViewsMap_->erase(currViewName);
                        allViewsNameList_->erase(allViewsNameList_->begin() + 2*i);
                        ImGui::EndPopup();
                        goto REDRAW;
                    }
                    ImGui::EndPopup();
                }
                
                const bool is_hovered = ImGui::IsItemHovered(); // Hovered
                const bool is_active = ImGui::IsItemActive();   // Held
                if (is_hovered && is_active && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                    activateView_ = allViewsMap_->at(currViewName);
                    isShowedView_ = true;
                }

                ImGui::Text("%s", currViewName.c_str());
            }
            
            ImGui::TableSetColumnIndex(1);
            if (i == tableRows-1 && isOdd) {
                break;
            }

            {
                currViewName = (allViewsMap_->at(allViewsNameList_->at(2*i + 1)).filepath).filename();

                iter = find(failedViewsNameList_->begin(), failedViewsNameList_->end(), currViewName);
                if (iter != failedViewsNameList_->end()) {
                    ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(255, 0, 0, 255));
                    ImGui::ImageButton((void *)(intptr_t)allViewsMap_->at(currViewName).texture,
                                        ImVec2(width / 2.5, width / 3.75));
                    ImGui::PopStyleColor();
                } else {
                    ImGui::ImageButton((void *)(intptr_t)allViewsMap_->at(currViewName).texture,
                                        ImVec2(width / 2.5, width / 3.75));
                }
                
                if (ImGui::BeginPopupContextItem()) {
                    if (ImGui::Selectable("删除")) {
                        // std::string deteleCMD = "rm " + (allViewsMap_[currViewName].filepath).string();
                        // system(deteleCMD.c_str());
                        allViewsMap_->erase(currViewName);
                        allViewsNameList_->erase(allViewsNameList_->begin() + 2*i + 1);
                        ImGui::EndPopup();
                        goto REDRAW;
                    }
                    ImGui::EndPopup();
                }

                const bool is_hovered = ImGui::IsItemHovered(); // Hovered
                const bool is_active = ImGui::IsItemActive();   // Held
                if (is_hovered && is_active && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                    activateView_ = allViewsMap_->at(currViewName);
                    isShowedView_ = true;
                }

                ImGui::Text("%s", currViewName.c_str());
            }
        }

        REDRAW: ImGui::EndTable();
    }
    ImGui::End();
}

void PhotoListPanel::OnImGuiShowView(int width, int height) {
    if (isShowedView_) {
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        bool is_visible = ImGui::Begin("查看图片窗口", &isShowedViewContext_);
        ImGui::SetWindowPos(ImVec2((center[0]-width), (center[1]-height)/2), ImGuiCond_Always);
        ImGui::SetWindowSize(ImVec2(activateView_.width * 1.05, activateView_.height * 1.05), ImGuiCond_Always);
        if (!isShowedViewContext_) {
            isShowedView_ = false;
            isShowedViewContext_ = true;
        } else if (is_visible) {
            ImGui::Image((void *)(intptr_t)activateView_.texture,
                                ImVec2(activateView_.width, activateView_.height));
        }
        ImGui::End();
    }
}