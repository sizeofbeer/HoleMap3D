#pragma once
#include "abcg.hpp"

#include <string>

class FileInfoPanel {
public:
    FileInfoPanel();
    ~FileInfoPanel();

    void OnImGui(int width, int height, int x, int y);
    inline void SetViewNums(int n_views);
    inline void SetViewPath(std::string viewsPath);
private:
    /* data */
    int n_views_;
    std::string viewsPath_;
};

inline void FileInfoPanel::SetViewNums(int n_views) {
    n_views_ = n_views;
}
inline void FileInfoPanel::SetViewPath(std::string viewsPath) {
    viewsPath_ = viewsPath;
}