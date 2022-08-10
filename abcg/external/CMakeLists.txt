cmake_minimum_required(VERSION 3.11)

project(external)

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(${PROJECT_NAME} SYSTEM
                           INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

if(NOT ENABLE_CONAN OR ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")

  if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    find_package(OpenGL REQUIRED)
    target_link_libraries(${PROJECT_NAME} INTERFACE OpenGL::GL)
  endif()

  add_subdirectory(cppitertools)
  add_subdirectory(fmt)
  add_subdirectory(glm)
  add_subdirectory(gsl)
  add_subdirectory(eigen)
  add_subdirectory(glew)
  add_subdirectory(SDL)
  add_subdirectory(SDL_image)
  add_subdirectory(imgui)


  target_include_directories(
    ${PROJECT_NAME} SYSTEM
    INTERFACE ${OPENGL_INCLUDE_DIR})

  target_link_libraries(${PROJECT_NAME} INTERFACE cppitertools fmt glm gsl
                                                  libglew_shared
                                                  SDL2
                                                  SDL2main
                                                  SDL2_image
                                                  imgui)
endif()
