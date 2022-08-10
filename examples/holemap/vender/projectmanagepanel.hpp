#pragma once
#include "abcg.hpp"

#include "sole.hpp"
#include "json.hpp"

#include <ImFileBrowser.h>
#include <string>
#include <fstream>
#include <filesystem>
class ProjectManagePanel
{
public:
    ProjectManagePanel();
    ~ProjectManagePanel();

    void OnImGui(int width, int height);
    void OnImGuiPopup(int width, int height);

    std::filesystem::path GetProjectPath();
    bool GetProjectLoaded();
    std::filesystem::path GetHistoryFilePath();
    inline bool GetIsNewCreated();

private:
    ImGui::FileBrowser* p_filebrowser_;
    std::filesystem::path p_path_;
    std::filesystem::path history_file_path_;
    bool p_loaded_;
    bool fb_opened_;
    bool isNewCreated_;
};

inline bool ProjectManagePanel::GetIsNewCreated() {
    return isNewCreated_;
}