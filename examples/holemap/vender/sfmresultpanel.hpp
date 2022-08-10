#pragma once
#include "abcg.hpp"

#include <string>
#include <vector>

class SFMResultPanel
{
public:
    SFMResultPanel();
    ~SFMResultPanel();

    void OnImGui(int width, int height);
    void SetPhotoTotalNums(int nums);
    void SetPhotoSFMFailedNums(int nums);
    inline bool GetIsStartMVS();
    inline void SetIsStartMVS(bool state);

private:
    int photo_total_nums_;
    int photo_sfmFailed_nums_;
    bool is_start_mvs_;
};

inline bool SFMResultPanel::GetIsStartMVS() {
    return is_start_mvs_;
}

inline void SFMResultPanel::SetIsStartMVS(bool state) {
    is_start_mvs_ = state;
}
