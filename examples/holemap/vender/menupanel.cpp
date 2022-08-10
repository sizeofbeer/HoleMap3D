#include "menupanel.hpp"

#include <unistd.h>
#include <opencv2/opencv.hpp>

bool LoadTextureFromFileByOpenCV(const char *filename,
                                GLuint *out_texture, int *out_width, int *out_height,
                                int zoom_width, int zoom_height) {
  cv::Mat imageMat = cv::imread(filename, 1);

  if (imageMat.cols <= 8 || imageMat.rows <= 8) {
    return false;
  }

  float zoom_factor = (float)zoom_width / zoom_height;
  float original_factor = (float)imageMat.cols / imageMat.rows;

  cv::Mat imageMatZoom, imageMatBd;
  if (imageMat.cols < imageMat.rows) {
    cv::resize(imageMat, imageMatZoom, cv::Size(zoom_width * 2, zoom_width * 2 / original_factor), 0, 0);
    int border = (imageMatZoom.rows * zoom_factor - imageMatZoom.cols) / 2;
    cv::copyMakeBorder(imageMatZoom, imageMatBd, 0, 0, border, border, cv::BORDER_CONSTANT, 0);
  } else {
    cv::resize(imageMat, imageMatZoom, cv::Size(zoom_width * 4, zoom_width * 4 / original_factor), 0, 0);
    if (zoom_factor < original_factor) {
      int border = (imageMatZoom.cols / zoom_factor - imageMatZoom.rows) / 2;
      cv::copyMakeBorder(imageMatZoom, imageMatBd, border, border, 0, 0, cv::BORDER_CONSTANT, 0);
    } else {
      imageMatBd = imageMatZoom;
    }
  }

  int image_width = imageMatBd.cols;
  int image_height = imageMatBd.rows;
  GLuint image_texture;
  int pixelNums = image_width * image_height * 3;
  GLubyte* pixels = new GLubyte[pixelNums];
  memcpy(pixels, imageMatBd.data, pixelNums * sizeof(unsigned char));

  glGenTextures(1, &image_texture);
  glBindTexture(GL_TEXTURE_2D, image_texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same


  // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height,
                              0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);
  free(pixels);

  *out_texture = image_texture;
  *out_width = image_width;
  *out_height = image_height;
  return true;
}

MenuPanel::MenuPanel() {
    isOpenedHelp_ = false;
    isShowedHelpContext_ = true;
    isLoadedPhoto_ = false;
    isOpenedOtherProject_ = false;
    mu_filebrowser_ = new ImGui::FileBrowser(ImGuiFileBrowserFlags_MultipleSelection);
    mu_filebrowser_->SetTitle("导入照片");
    mu_filebrowser_->SetTypeFilters({".JPG", ".jpg", ".PNG", ".png"});
}

MenuPanel::~MenuPanel() {}

void MenuPanel::OnImGui(int width, int height)
{
    ImGuiWindowFlags flags{ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDecoration | \
                            ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove};
    ImGui::Begin("菜单", nullptr, flags);
    ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetWindowSize(ImVec2(width, height), ImGuiCond_Always);

    ImGui::BeginMenuBar();
    if (ImGui::BeginMenu("文件"))
    {
        if(ImGui::MenuItem("导入图片", nullptr, &isLoadedPhoto_)) {
            mu_filebrowser_->Open();
        }
        if(ImGui::MenuItem("退出", nullptr)) {
            exit(0);
        }
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("帮助"))
    {
        ImGui::MenuItem("使用说明", nullptr, &isOpenedHelp_);
        ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
    ImGui::End();
}

void MenuPanel::GetLoadedPhotosPathByFB(std::filesystem::path historyFilePath,
                                std::filesystem::path projectPath,
                                std::map<std::string, class ImageView> &allViewsMap,
                                std::vector<std::string> &allViewsNameList,
                                int width, int height) {
  mu_filebrowser_->Display();
  if (mu_filebrowser_->HasSelected()) {
    std::vector<std::filesystem::path> photosPath = mu_filebrowser_->GetMultiSelected();
    mu_filebrowser_->ClearSelected();
    GetLoadedPhotosPath(projectPath, photosPath, allViewsMap, allViewsNameList, width, height);
    std::ofstream o(historyFilePath);
    nlohmann::json history;
    history["projectPath"] = projectPath.string();
    for (auto ViewMap : allViewsMap) {
      history["photosPath"].push_back(ViewMap.second.filepath.string());
    }
    o << std::setw(4) << history << std::endl;
  } else if (!mu_filebrowser_->IsOpened()) {
    isLoadedPhoto_ = false;
  }
}

void MenuPanel::GetLoadedPhotosPath(std::filesystem::path projectPath,
                                std::vector<std::filesystem::path> photosPath,
                                std::map<std::string, class ImageView> &allViewsMap,
                                std::vector<std::string> &allViewsNameList,
                                int width, int height) {
  auto photoSymsDirPath = joinPaths(projectPath, "symlinks");
  if (!std::filesystem::exists(photoSymsDirPath)) {
      std::filesystem::create_directory(photoSymsDirPath);
  }

  std::map<std::string, class ImageView>::iterator iter;
  for (auto photoPath : photosPath) {
    if (photoPath.filename().extension() != ".JPG" && \
        photoPath.filename().extension() != ".jpg" && \
        photoPath.filename().extension() != ".PNG" && \
        photoPath.filename().extension() != ".png") {
      continue;
    }

    iter = allViewsMap.find(photoPath.filename().string());
    if(iter != allViewsMap.end()) continue;

    std::string photosymPath = joinPaths(photoSymsDirPath, photoPath.filename().string());
    symlink(photoPath.string().c_str(), photosymPath.c_str());

    ImageView image{};
    LoadTextureFromFileByOpenCV(photosymPath.c_str(),
                                &(image.texture), &(image.width), &(image.height),
                                width, height);
    image.filepath = photosymPath.c_str();

    allViewsNameList.push_back(photoPath.filename().string());
    allViewsMap.emplace(photoPath.filename().string(), image);
  }
  isLoadedPhoto_ = false;
}

void MenuPanel::OnImGuiHelpWindow(int width, int height) {
    if (isOpenedHelp_) {
        ImGuiWindowFlags flags{ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse};
        if (!ImGui::Begin("帮助页面", &isShowedHelpContext_, flags) || !isShowedHelpContext_)
        {
            isShowedHelpContext_ = true;
            isOpenedHelp_ = false;
            ImGui::End();
            return;
        }
      
        ImGui::SetWindowSize(ImVec2(width, height));

        ImGui::Text("常见问题");
        ImGui::BeginGroup();
        ImGui::Text("问题: 怎么开始重建物体?");
        ImGui::BulletText("[文件]->[导入图片]->[点云生成]\n"
                          "先导入图片, 然后生成稀疏点云.");
        ImGui::Text("问题: 如何提高模型重建效果?");
        ImGui::BulletText("稀疏点云很大程度影响模型拓扑的效果, 只需保证稀疏点云的分布均匀.");
        ImGui::Text("问题: 点云生成/模型拓扑后的效果不满意, 怎么操作?");
        ImGui::BulletText("[初始化]->[文件]->[导入图片]->[点云生成]\n"
                          "先初始化pipeline, 然后增加额外图片来重新生成点云.");
        ImGui::EndGroup();
        ImGui::End();
    }
}