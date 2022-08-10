#pragma once
#include <glm/vec4.hpp>
#include <eigen/Eigen/Core>


const float kFieldOfView = 25.0f;
const glm::vec4 kSelectedImagePlaneColor{0.7f, 0.5f, 0.5f, 1.0f};
const glm::vec4 kSelectedImageFrameColor{0.5f, 0.6f, 0.5f, 1.0f};
const glm::vec4 kGridColor{0.2f, 0.2f, 0.2f, 0.5f};
const glm::vec4 kXAxisColor{0.9f, 0.0f, 0.0f, 0.5f};
const glm::vec4 kYAxisColor{0.0f, 0.9f, 0.0f, 0.5f};
const glm::vec4 kZAxisColor{0.0f, 0.0f, 0.9f, 0.5f};

struct ImageView
{
    GLuint texture{};
    std::filesystem::path filepath{};
    int width{};
    int height{};
};