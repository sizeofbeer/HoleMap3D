#pragma once

#include "abcg.hpp"

#include "trackball.hpp"
#include "sfmpainter.h"
#include "gridpainter.h"
#include "meshpainter.h"

#include "projectmanagepanel.hpp"
#include "menupanel.hpp"
#include "photolistpanel.hpp"
#include "quickstartpanel.hpp"
#include "sfmresultpanel.hpp"
#include "fileinfopanel.hpp"
#include "logpanel.hpp"
#include "attributepanel.hpp"

#include <map>

using AlicePipe = HoleMap::AlicePipeline;

class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void handleEvent(SDL_Event& ev) override;
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;
  void resizeGL(int width, int height) override;
  void terminateGL() override;

  void Upload();
  void Render();
  void setupView();

 private:
  // Shaders
  std::vector<const char*> m_shaderNames{"UnlitVertexColor", "texture"};
  std::vector<GLuint> m_programs;
  int m_currentProgramIndex{0};
  int m_currentShowObject{0}; // 0: None, 1: sfm, 2: textured mesh

  bool is_loaded{false};

  float m_zoom{};
  int m_viewportWidth{800};
  int m_viewportHeight{800};

  std::array<float, 4> m_clearColor{0.0f, 0.0f, 0.0f, 0.0f};
  
  glm::mat4x4 model_matrix_;
  glm::mat4x4 view_matrix_;
  glm::mat4x4 proj_matrix_;

  HoleMap::MeshPainter mesh_painter_;
  HoleMap::GridPainter grid_painter_;
  HoleMap::SFMPainter sfm_painter_;

  TrackBall m_trackModel;

  std::filesystem::path projectPath_;
  std::string sfmJsonPath_;
  std::string objPath_;
  std::map<std::string, class ImageView> allViewsMap_;
  std::vector<std::string> allViewsNameList_;
  std::vector<std::string> failedViewsNameList_;

  ProjectManagePanel p_manage_panel_;
  MenuPanel mu_panel_;
  PhotoListPanel ptl_panel_;
  QuickStartPanel qks_panel_;
  SFMResultPanel sfm_ResultPanel_;
  FileInfoPanel fi_panel_;
  LogPanel log_panel_;
  AttributePanel att_panel_;
};