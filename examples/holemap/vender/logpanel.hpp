#pragma once
#include "abcg.hpp"

#include <string>

class LogPanel {
public:
    LogPanel();
    ~LogPanel();

    void OnImGui(int width, int height, int x, int y);
    inline void SetLogInfo(std::string info);
private:
    std::string logInfo_;
};

inline void LogPanel::SetLogInfo(std::string info) {
    logInfo_ = info;
}