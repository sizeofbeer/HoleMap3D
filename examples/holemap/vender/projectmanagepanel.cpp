#include "projectmanagepanel.hpp"

ProjectManagePanel::ProjectManagePanel() {
    p_loaded_ = false;
    fb_opened_ = false;
    isNewCreated_ = true;

    p_filebrowser_ = new ImGui::FileBrowser(
                            ImGuiFileBrowserFlags_SelectDirectory | \
                            ImGuiFileBrowserFlags_CreateNewDir);
    p_filebrowser_->SetTitle("选择项目文件夹");
    p_filebrowser_->SetTypeFilters({".*"});
}

ProjectManagePanel::~ProjectManagePanel() {}

void ProjectManagePanel::OnImGuiPopup(int width, int height) {
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    if (ImGui::BeginPopupModal("ProjectPopUpWindow", NULL, ImGuiWindowFlags_NoResize)) {
        ImGui::SetWindowPos(ImVec2(center[0] - width/2, center[1] - height/2));
        ImGui::SetWindowSize(ImVec2(width, height));

        ImGui::Text("%s", "是否创建新项目");
        if (ImGui::Button("是", ImVec2(120, 0)))
        {
            sole::uuid u4 = sole::uuid4();
            p_path_ = p_filebrowser_->GetSelected() / u4.str();
            if (!std::filesystem::exists(p_path_)) std::filesystem::create_directory(p_path_);
            p_filebrowser_->ClearSelected();
            p_loaded_ = true;
            fb_opened_ = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::SetItemDefaultFocus();
        ImGui::SameLine();
        if (ImGui::Button("否", ImVec2(120, 0)))
        {
            p_filebrowser_->ClearSelected();
            p_loaded_ = false;
            fb_opened_ = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void ProjectManagePanel::OnImGui(int width, int height) {
    // Always center this window when appearing
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();

    ImGuiWindowFlags flags{ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | \
                            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove};
    ImGui::Begin("项目管理", nullptr, flags);
    ImGui::SetWindowPos(ImVec2(center[0] - width/2, center[1] - height/2));
    ImGui::SetWindowSize(ImVec2(width, height));

    ImGui::Text("请点击\"新建/打开\"项目");
    ImGui::BeginGroup();
    ImGui::Dummy(ImVec2(15, 15)); // 占位空格
    ImGui::EndGroup();
    ImGui::BeginGroup();

    ImGui::Dummy(ImVec2(30, 0));
    ImGui::SameLine();
    if(ImGui::Button("新建", ImVec2(60, 30))) {
        p_filebrowser_->Open();
        fb_opened_ = true;
        isNewCreated_ = true;
    }
    ImGui::SameLine();
    ImGui::Dummy(ImVec2(width-200-30, 0)); // 占位空格
    ImGui::SameLine();
    if (ImGui::Button("打开",ImVec2(60, 30))) {
        p_filebrowser_->Open();
        fb_opened_ = true;
        isNewCreated_ = false;
    }

    ImGui::EndGroup();
    ImGui::End();

    p_filebrowser_->Display();
    
    if (p_filebrowser_->HasSelected()) {
        history_file_path_ = p_filebrowser_->GetSelected() / "hole.json";
        if (isNewCreated_ && std::filesystem::exists(history_file_path_)) {
            ImGui::OpenPopup("ProjectPopUpWindow");
            return;
        } else if (!isNewCreated_ && !std::filesystem::exists(history_file_path_)) {
            ImGui::OpenPopup("ProjectPopUpWindow");
            return;
        }

        if (isNewCreated_) {
            sole::uuid u4 = sole::uuid4();
            p_path_ = p_filebrowser_->GetSelected() / u4.str();
            if (!std::filesystem::exists(p_path_)) std::filesystem::create_directory(p_path_);
        } else {
            std::ifstream fs(history_file_path_);
            nlohmann::json history;
            fs >> history;
            p_path_ = history["projectPath"].get<std::string>();
        }
        p_filebrowser_->ClearSelected();
        p_loaded_ = true;
        fb_opened_ = false;
    } else if (!p_filebrowser_->IsOpened()) {
        if (fb_opened_) {
            p_loaded_ = false;
            fb_opened_ = false;
        }
    }
}

std::filesystem::path ProjectManagePanel::GetProjectPath() {
    return p_path_;
}

std::filesystem::path ProjectManagePanel::GetHistoryFilePath() {
    return history_file_path_;
}

bool ProjectManagePanel::GetProjectLoaded() {
    return p_loaded_;
}
