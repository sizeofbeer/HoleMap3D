cmake_minimum_required(VERSION 3.5)

project(imgui)

add_library(
  ${PROJECT_NAME}
  imgui.cpp
  imgui_demo.cpp
  imgui_draw.cpp
  imgui_tables.cpp
  imgui_widgets.cpp
  imgui_impl_sdl.cpp
  imgui_impl_opengl3.cpp)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
  set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-s USE_SDL=2")
else()
  target_link_libraries(${PROJECT_NAME} PRIVATE SDL2
                                                SDL2main)
endif()
