#pragma once
#include "abcg.hpp"

#include "params.h"
#include "misc.hpp"
#include "json.hpp"

#include <map>
#include <thread>
#include <fstream>
#include <ImFileBrowser.h>

class MenuPanel
{
public:

    MenuPanel();
    ~MenuPanel();

    void OnImGui(int width, int height);
    void OnImGuiHelpWindow(int width, int height);

    void GetLoadedPhotosPathByFB(std::filesystem::path historyFilePath,
                                std::filesystem::path projectPath,
                                std::map<std::string, class ImageView> &allViewsMap,
                                std::vector<std::string> &allViewsNameList,
                                int width, int height);

    void GetLoadedPhotosPath(std::filesystem::path projectPath,
                                std::vector<std::filesystem::path> photosPath,
                                std::map<std::string, class ImageView> &allViewsMap,
                                std::vector<std::string> &allViewsNameList,
                                int width, int height);

    inline bool GetIsOpenedHelp();
    inline bool GetIsOpenedOtherProject();
    inline bool GetIsLoadedPhoto();
private:
    bool isOpenedHelp_;
    bool isShowedHelpContext_;
    bool isOpenedOtherProject_;
    bool isLoadedPhoto_;
    ImGui::FileBrowser* mu_filebrowser_;
};

inline bool MenuPanel::GetIsOpenedHelp() {
    return isOpenedHelp_;
}

inline bool MenuPanel::GetIsOpenedOtherProject() {
    return isOpenedOtherProject_;
}

inline bool MenuPanel::GetIsLoadedPhoto() {
    return isLoadedPhoto_;
}