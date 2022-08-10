#include <fmt/core.h>
#include <eigen/Eigen/Core>

#include "abcg.hpp"
#include "openglwindow.hpp"

#include "sfmDataIOFromMeshroom.hpp"
#include "sfmDataIOFromColmap.hpp"
#include "misc.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <filesystem>

int main(int argc, char **argv) {
  try {
    // Create application instance
    abcg::Application app(argc, argv);

    // Create OpenGL window
    auto window{std::make_unique<OpenGLWindow>()};
    window->setOpenGLSettings({.profile = abcg::OpenGLProfile::Core,
                               .majorVersion = 4,
                               .minorVersion = 1});
    window->setWindowSettings(
        {
          .width = 1920,
          .height = 1080,
          .showFPS=false,
          .showFullscreenButton=false,
          .title = "HOLEMAP: 3D物体视觉重建系统"});

    // Run application
    app.run(std::move(window));
    
  } catch (const abcg::Exception &exception) {
    fmt::print(stderr, "{}", exception.what());
    return -1;
  }
  return 0;
}