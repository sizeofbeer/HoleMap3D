#include "openglwindow.hpp"

#include "sfmDataIOFromMeshroom.hpp"

#include <gsl/gsl>
#include <fstream>

void findFailReconstructImage(std::filesystem::path jsonfilepath,
                              std::vector<std::string> &failPhotoNames) {
  std::ifstream fs(jsonfilepath);
  nlohmann::json sfm;
  fs >> sfm;
  // step1: 获取所有成功重建的pose
  std::vector<std::string> poseIDs;

  for (auto pose : sfm["poses"]) {
      poseIDs.push_back(pose["poseId"]);
  }
  // 筛选出未成功重建的pose并给出图片

  for (auto view : sfm["views"]) {
      if (std::find(poseIDs.begin(), poseIDs.end(), view["poseId"]) != poseIDs.end())
          continue;
      std::filesystem::path image_path = view["path"];
      failPhotoNames.push_back(image_path.filename().string());
  }
}

void OpenGLWindow::handleEvent(SDL_Event& event) {
  glm::ivec2 mousePosition;
  SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

  if (event.type == SDL_MOUSEMOTION) {
    m_trackModel.mouseMove(mousePosition);
  }
  if (event.type == SDL_MOUSEBUTTONDOWN) {
    if (event.button.button == SDL_BUTTON_LEFT) {
      m_trackModel.mousePress(mousePosition);
    }
  }
  if (event.type == SDL_MOUSEBUTTONUP) {
    if (event.button.button == SDL_BUTTON_LEFT) {
      m_trackModel.mouseRelease(mousePosition);
    }
  }

  if (event.type == SDL_MOUSEWHEEL) {
    m_zoom += (event.wheel.y > 0 ? 1.0f : -1.0f) / 5.0f;
    // m_zoom = glm::clamp(m_zoom, -1.5f, 1.0f);
  }
}

void OpenGLWindow::initializeGL() {
  glEnable(GL_DEPTH_TEST);

  // Create programs
  for (const auto& name : m_shaderNames) {
    auto path{getAssetsPath() + name};
    auto program{createProgramFromFile(path + ".vert", path + ".frag")};
    m_programs.push_back(program);
  }

  m_trackModel.setAxis(glm::normalize(glm::vec3(-0.1, 1, 0.05)));
}

void OpenGLWindow::paintGL() {
  // Set the clear color
  setupView();

  glClearColor(gsl::at(m_clearColor, 0), gsl::at(m_clearColor, 1),
               gsl::at(m_clearColor, 2), gsl::at(m_clearColor, 3));
  // Clear the color buffer and Z-buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // viewport
  glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  Render();
}

void OpenGLWindow::paintUI() {
  // Parent class will show fullscreen button and FPS meter
  abcg::OpenGLWindow::paintUI();
  
  // ImGui::ShowDemoWindow();
  p_manage_panel_.OnImGuiPopup(320, 100);
  if (!p_manage_panel_.GetProjectLoaded()) {
    p_manage_panel_.OnImGui(320, 100);
  }
  
  if (!std::filesystem::exists(p_manage_panel_.GetProjectPath())) { 
    return;
  }

  projectPath_ = p_manage_panel_.GetProjectPath();
  std::filesystem::path historyFilePath = p_manage_panel_.GetHistoryFilePath();

  float att_x = m_viewportWidth/4 * 3;
  float att_h = (m_viewportHeight-24)/4 * 3;
  float att_w = m_viewportWidth / 4;
  att_panel_.OnImGui(att_w, att_h, att_x, 24);
  qks_panel_.SetExecPatten(att_panel_.GetImageMatchType());

  sfmJsonPath_ = joinPaths(projectPath_, "StructureFromMotion/sfm.json");
  objPath_ = joinPaths(projectPath_, "Texturing/texturedMesh.obj");

  mu_panel_.OnImGui(m_viewportWidth, 0);
  mu_panel_.OnImGuiHelpWindow(400, 500);

  if(!p_manage_panel_.GetIsNewCreated() && !is_loaded && std::filesystem::exists(historyFilePath)) {
    std::vector<std::filesystem::path> tmpPathList;
    std::ifstream fs(historyFilePath);
    nlohmann::json history;
    fs >> history;
    for (auto photoPath : history["photosPath"]) {
      tmpPathList.push_back(photoPath.get<std::string>());
    }
    mu_panel_.GetLoadedPhotosPath(projectPath_, tmpPathList, allViewsMap_, allViewsNameList_,
                                              m_viewportWidth / 10, m_viewportWidth / 15);

    if (std::filesystem::exists(objPath_)) {
      m_currentProgramIndex = 1;
      att_panel_.SetViewObjectType(2);
    } else if (std::filesystem::exists(sfmJsonPath_)) {
      m_currentProgramIndex = 0;
      att_panel_.SetViewObjectType(1);
    } else {
      m_currentProgramIndex = 0;
      att_panel_.SetViewObjectType(0);
    }
    m_currentShowObject = att_panel_.GetViewObjectType();
    Upload();
    is_loaded = true;
  }

  mu_panel_.GetLoadedPhotosPathByFB(historyFilePath, projectPath_, allViewsMap_, allViewsNameList_,
                                              m_viewportWidth / 10, m_viewportWidth / 15);

  sfm_ResultPanel_.OnImGui(320, 100);
  sfm_ResultPanel_.SetPhotoSFMFailedNums(failedViewsNameList_.size());
  sfm_ResultPanel_.SetPhotoTotalNums(allViewsNameList_.size());

  if (sfm_ResultPanel_.GetIsStartMVS()) {
    qks_panel_.SetExecPatten(2);
    qks_panel_.StartMVS();
    sfm_ResultPanel_.SetIsStartMVS(false);
  }

  ptl_panel_.SetAllViewsMap(&allViewsMap_);
  ptl_panel_.SetAllViewsNameList(&allViewsNameList_);
  ptl_panel_.SetFailedViewsNameList(&failedViewsNameList_);
  ptl_panel_.OnImGui(m_viewportWidth / 4, m_viewportHeight);
  ptl_panel_.OnImGuiShowView(m_viewportWidth / 4, m_viewportHeight / 4);

  bool is_photo_existed = allViewsNameList_.size() != 0 ? true : false;
  qks_panel_.SetIsExistPhoto(is_photo_existed);
  qks_panel_.OnImGui(m_viewportWidth/4, m_viewportHeight-24);
  // "../../../aliceVision"
  qks_panel_.LoadPipeLineConfig("../../../aliceVision", projectPath_.string(), allViewsNameList_,
                                att_panel_.GetFeatureExtractLevel(), att_panel_.GetSeqLength());
  int currFlow = qks_panel_.ExecPipeLine();

  if (currFlow >= 1) {
    att_panel_.SetViewObjectType(currFlow);
    m_currentShowObject = att_panel_.GetViewObjectType();
    m_currentProgramIndex = currFlow > 1 ? 1 : 0;
    Upload();
    qks_panel_.SetIsUploaded(true);
  } else if (currFlow != 0) {
    m_currentShowObject = att_panel_.GetViewObjectType();
    m_currentProgramIndex = m_currentShowObject > 1 ? 1 : 0;
    if ((mesh_painter_.getNumTriangles() == 0 && m_currentShowObject == 2) || \
        (sfm_painter_.getNumGeoms() == 0 && m_currentShowObject == 1) || \
        (grid_painter_.getNumGeoms() == 0 && m_currentShowObject == 0)) {
      Upload();
    }
  }

  fi_panel_.SetViewPath(projectPath_);
  fi_panel_.SetViewNums((int)allViewsNameList_.size());
  float fi_x = m_viewportWidth / 4;
  float fi_width = fi_x * 2;
  float fi_height = (m_viewportHeight-24) / 5;
  float fi_y = 24 + fi_height*4;
  fi_panel_.OnImGui(fi_width, fi_height, fi_x, fi_y);

  float log_width = m_viewportWidth / 4;
  float log_height = (m_viewportHeight-24) / 4;
  float log_y = 24 + log_height*3;
  float log_x = log_width * 3;
  log_panel_.OnImGui(log_width, log_height, log_x, log_y);
}

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;
  m_trackModel.resizeViewport(width, height);
}

void OpenGLWindow::terminateGL() {
  // Release OpenGL resources
  for (const auto& program : m_programs) {
    glDeleteProgram(program);
  }
}

void OpenGLWindow::setupView() {
  auto aspect{static_cast<float>(m_viewportWidth) /
                  static_cast<float>(m_viewportHeight)};
  proj_matrix_ =
          // glm::ortho(-1.0f * aspect, 1.0f * aspect, -1.0f, 1.0f, 0.1f, 5.0f);
          glm::perspective(glm::radians(45.0f), aspect, 0.1f, 5.0f);

  model_matrix_ = m_trackModel.getRotation();

  view_matrix_ =
      glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f + m_zoom),
                  glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

void OpenGLWindow::Upload() {
  if (m_currentShowObject == 2) {
    mesh_painter_.Setup(m_programs.at(m_currentProgramIndex));
    mesh_painter_.Upload(objPath_);
    failedViewsNameList_.clear();
    findFailReconstructImage(sfmJsonPath_, failedViewsNameList_);
  } else if (m_currentShowObject == 1) {
    sfm_painter_.Setup(m_programs.at(m_currentProgramIndex));
    HoleMap::MeshroomSFMDataIO sfmDataIOFromMeshroom;
    sfmDataIOFromMeshroom.loadSFMFromJSON(sfmJsonPath_);
    sfm_painter_.Upload(sfmDataIOFromMeshroom.Points3D(),
                            sfmDataIOFromMeshroom.Images(),
                            sfmDataIOFromMeshroom.Cameras());
    failedViewsNameList_.clear();
    findFailReconstructImage(sfmJsonPath_, failedViewsNameList_);
  } else {
    grid_painter_.Setup(m_programs.at(m_currentProgramIndex));
    grid_painter_.Upload();
  }
  
}

void OpenGLWindow::Render() {
  glm::mat4x4 pmv_matrix = proj_matrix_ * view_matrix_ * model_matrix_;
  if (m_currentShowObject == 2) {
    mesh_painter_.Render(model_matrix_, view_matrix_, proj_matrix_);
  } else if (m_currentShowObject == 1) {
    sfm_painter_.Render(pmv_matrix, 3, 5);
  } else {
    grid_painter_.Render(pmv_matrix, 5);
  }
}