#pragma once
#include "abcg.hpp"

#include <string>

static const char* extractItems[] = {"normal", "high"};

class AttributePanel {
public:
    AttributePanel();
    ~AttributePanel();

    void OnImGui(int width, int height, int x, int y);
    inline int GetImageMatchType();
    inline int GetViewObjectType();
    inline void SetViewObjectType(int type);
    inline std::string GetFeatureExtractLevel();
    inline int GetSeqLength();

private:
    /* data */
    int featureExtractLevel_;
    int seqLength_;
    int imageMatchType_;
    int viewObjectType_;
};

inline int AttributePanel::GetImageMatchType() {
    return imageMatchType_;
}
inline int AttributePanel::GetViewObjectType() {
    return viewObjectType_;
}
inline std::string AttributePanel::GetFeatureExtractLevel() {
    return extractItems[featureExtractLevel_];
}
inline int AttributePanel::GetSeqLength() {
    return seqLength_;
}

inline void AttributePanel::SetViewObjectType(int type) {
    viewObjectType_ = type;
}