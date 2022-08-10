#pragma once
#include "abcg.hpp"

#include "params.h"

#include <map>
#include <filesystem>

class PhotoListPanel {
public:
    PhotoListPanel();
    ~PhotoListPanel();

    void OnImGui(int width, int height);
    void OnImGuiShowView(int width, int height);

    inline void SetAllViewsMap(std::map<std::string, class ImageView> *viewsMap);
    inline void SetAllViewsNameList(std::vector<std::string> *viewsNameList);
    inline void SetFailedViewsNameList(std::vector<std::string> *failedViewsNameList);

private:
    ImageView activateView_;
    bool isShowedView_;
    bool isShowedViewContext_;
    std::map<std::string, class ImageView>* allViewsMap_;
    std::vector<std::string>* allViewsNameList_;
    std::vector<std::string>* failedViewsNameList_;
};

inline void PhotoListPanel::SetAllViewsMap(std::map<std::string, class ImageView> *viewsMap) {
    allViewsMap_ = viewsMap;
}

inline void PhotoListPanel::SetAllViewsNameList(std::vector<std::string> *viewsNameList) {
    allViewsNameList_ = viewsNameList;
}

inline void PhotoListPanel::SetFailedViewsNameList(std::vector<std::string> *failedViewsNameList) {
    failedViewsNameList_ = failedViewsNameList;
}
