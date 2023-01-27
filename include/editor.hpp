#ifndef __EDITOR_H__
#define __EDITOR_H__

#include "light_sources.hpp"
#include "stage.hpp"
#include "batcher.hpp"

#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

void initialize_imgui(GLFWwindow *window) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;

  ImGui::StyleColorsDark();
  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, 6.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{8.0f, 6.0f});
  ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2{10.0f, 16.0f});
  ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize, 11.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 2.0f);
  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330");
}

void tree_directional(const char *label, DirectionalLight *directional_light) {
  if (ImGui::TreeNode(label)) {
    ImGui::Checkbox("Enabled", (bool *)(&directional_light->enabled));
    ImGui::DragFloat3("Direction", (float *)(&directional_light->direction), 0.01f, -1.0f, 1.0f);
    ImGui::ColorEdit3("Color", (float *)(&directional_light->color));
    ImGui::SliderFloat("Ambient Strength", (float *)(&directional_light->ambient_strength), 0.0f, 1.0f);
    ImGui::SliderFloat("Diffuse Strength", (float *)(&directional_light->diffuse_strength), 0.0f, 10.0f);
    ImGui::SliderFloat("Specular Strength", (float *)(&directional_light->specular_strength), 0.0f, 10.0f);
    ImGui::Separator();
    ImGui::TreePop();
  }
}

void tree_points(const char *label, PointLight *point_light) {
  if (ImGui::TreeNode(label)) {
    ImGui::Checkbox("Enabled", (bool *)(&point_light->enabled));
    ImGui::DragFloat3("Position", (float *)(&point_light->position), 0.1f);
    ImGui::ColorEdit3("Color", (float *)(&point_light->color));
    ImGui::SliderFloat("Ambient Strength", (float *)(&point_light->ambient_strength), 0.0f, 1.0f);
    ImGui::SliderFloat("Diffuse Strength", (float *)(&point_light->diffuse_strength), 0.0f, 10.0f);
    ImGui::SliderFloat("Specular Strength", (float *)(&point_light->specular_strength), 0.0f, 10.0f);
    if (ImGui::TreeNode("Attenuation")) {
      ImGui::TextColored(ImVec4{0.8f, 0.4f, 0.20f, 1.0f}, "! The distance traveled by the light.");
      ImGui::TextColored(ImVec4{0.8f, 0.4f, 0.20f, 1.0f}, "! Stronger lights have a significantly lower quadratic value.");
      ImGui::SliderFloat("Linear", (float *)(&point_light->linear), 0.0f, 1.0f, "%f");
      ImGui::SliderFloat("Quadratic", (float *)(&point_light->quadratic), 0.0f, 2.0f, "%f");
      ImGui::TreePop();
    }
    ImGui::Separator();
    ImGui::TreePop();
  }
}

void tree_spot(const char *label, SpotLight *spot_light) {
  if (ImGui::TreeNode(label)) {
    ImGui::Checkbox("Enabled", (bool *)(&spot_light->enabled));
    ImGui::DragFloat3("Position", (float *)(&spot_light->position), 0.1f);
    ImGui::DragFloat3("Direction", (float *)(&spot_light->direction), 0.01f, -1.0f, 1.0f);
    ImGui::ColorEdit3("Color", (float *)(&spot_light->color));
    ImGui::SliderFloat("Ambient Strength", (float *)(&spot_light->ambient_strength), 0.0f, 1.0f);
    ImGui::SliderFloat("Diffuse Strength", (float *)(&spot_light->diffuse_strength), 0.0f, 10.0f);
    ImGui::SliderFloat("Specular Strength", (float *)(&spot_light->specular_strength), 0.0f, 10.0f);
    if (ImGui::TreeNode("Attenuation")) {
      ImGui::TextColored(ImVec4{0.8f, 0.4f, 0.20f, 1.0f}, "! The distance traveled by the light.");
      ImGui::TextColored(ImVec4{0.8f, 0.4f, 0.20f, 1.0f}, "! Stronger lights have a significantly lower quadratic value.");
      ImGui::SliderFloat("Linear", (float *)(&spot_light->linear), 0.0f, 1.0f, "%f");
      ImGui::SliderFloat("Quadratic", (float *)(&spot_light->quadratic), 0.0f, 2.0f, "%f");
      ImGui::TreePop();
    }
    if (ImGui::TreeNode("Intensity")) {
      ImGui::TextColored(ImVec4{0.8f, 0.4f, 0.20f, 1.0f}, "! A large difference in values will result in smoother edges.");

      float initial_inner = {spot_light->cutoff};
      float initial_outer = {spot_light->outer_cutoff};

      if (initial_outer < initial_inner) {
        spot_light->outer_cutoff = spot_light->cutoff;
      }
      ImGui::SliderFloat("Inner Cutoff", (float *)(&spot_light->cutoff), 0.0f, 100.0f);
      ImGui::SliderFloat("Outer Cutoff", (float *)(&spot_light->outer_cutoff), 0.0f, 100.0f);
      if (initial_outer < initial_inner) {
        spot_light->cutoff = spot_light->outer_cutoff;
      }
      ImGui::TreePop();
    }
    ImGui::Separator();
    ImGui::TreePop();
  }
}

void render_imgui(Stage &stage, const Statistics &batcher_stats) {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  // bool show_demo_window = true;
  // ImGui::ShowDemoWindow(&show_demo_window);

  ImGuiWindowFlags window_flags = 0;
  window_flags |= ImGuiWindowFlags_NoTitleBar;
  window_flags |= ImGuiWindowFlags_NoBackground;
  window_flags |= ImGuiWindowFlags_NoMove;
  window_flags |= ImGuiWindowFlags_NoResize;

  stage.imgui_hovering = ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);

  ImGui::Begin("Editor", nullptr, window_flags);

  ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
  // ImGui::TextColored(ImVec4{0.2f, 0.8f, 0.5f, 1.0f}, "STATISTICS");
  ImGui::TextColored(ImVec4{0.4f, 0.7f, 0.3f, 1.0f}, "Camera Pos:  (X) %.1f (Y) %.1f (Z) %.1f ", stage.camera.Position.x,
                     stage.camera.Position.y, stage.camera.Position.z);
  ImGui::TextColored(ImVec4{0.8f, 0.2f, 0.3f, 1.0f}, "Screen Blits: %i", batcher_stats.flushes);
  ImGui::TextColored(ImVec4{0.8f, 0.2f, 0.3f, 1.0f}, "VertexCount: %i", batcher_stats.scene_vertices);
  ImGui::TextColored(ImVec4{0.8f, 0.2f, 0.3f, 1.0f}, "Unique Textures: %i", batcher_stats.unique_textures);
  // ImGui::TextColored(ImVec4{0.8f, 0.2f, 0.9f, 1.0f}, "Rebinds: %i", batcher_stats.);

  if (!stage.show_gui) {
    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    return;
  }

  ImGui::Separator();
  ImGui::TextColored(ImVec4{0.8f, 0.4f, 0.74f, 1.0f}, "HINTS: Press C to toggle cursor | WASD to move | SPACE to elevate");
  ImGui::TextColored(ImVec4{0.8f, 0.4f, 0.74f, 1.0f}, "HINTS: Arrow Keys & Q / E controls the first spot light");

  ImGui::Separator();
  ImGui::TextColored(ImVec4{0.8f, 0.4f, 1.00f, 1.0f}, "AMBIENT: Anything in range of the light (directional is global)");
  ImGui::TextColored(ImVec4{0.8f, 0.4f, 1.00f, 1.0f}, "DIFFUSE: Color intensity affected by the angle of the light");
  ImGui::TextColored(ImVec4{0.8f, 0.4f, 1.00f, 1.0f}, "SPECULAR: Shiny or glosiness effect perceived by a view space angle");
  ImGui::TextColored(ImVec4{0.8f, 0.4f, 0.20f, 1.0f},
                     "! By Default: All emission is affected by diffuse angle and color per light.");
  ImGui::TextColored(ImVec4{0.8f, 0.4f, 0.20f, 1.0f}, "! What may appear to be shadows is linear mipmapping over steel borders.");

  ImGui::Separator();
  ImGui::ColorEdit3("Clear Color", (float *)&stage.clear_);
  ImGui::SliderFloat("Camera Speed", &stage.move_speed, 0.0f, 50.0f);
  ImGui::SliderFloat("Material Shine", &stage.material_shininess, 0.0f, 1.0f);
  ImGui::SliderFloat("Emission Speed", &stage.emission_speed, 0.0f, 10.0f);
  ImGui::SliderFloat("Emission Strength", &stage.emission_strength, 0.0f, 10.0f);

  ImGui::Separator();
  if (ImGui::CollapsingHeader("Model Properties")) {
    ImGui::TextColored(ImVec4{0.8f, 0.4f, 0.20f, 1.0f},
                       "! Models won't always contain specified masks; \nBut also won't cause errors if not supported.");
    ImGui::Checkbox("With Diffuse", &stage.backpack.has_diffuse);
    ImGui::Checkbox("With Specular", &stage.backpack.has_specular);
    ImGui::Checkbox("With Emission", &stage.backpack.has_emission);
  }

  if (ImGui::CollapsingHeader("Directional Lighting")) {
    for (size_t i{0}; i < 1; i++) {
      tree_directional(("Directional Light #" + std::to_string(i + 1)).c_str(), &stage.dir_lights[i]);
    }
  }
  if (ImGui::CollapsingHeader("Spot Lighting")) {
    for (size_t i{0}; i < 1; i++) {
      tree_spot(("Spot Light #" + std::to_string(i + 1)).c_str(), &stage.spot_lights[i]);
    }
  }
  if (ImGui::CollapsingHeader("Point Lighting")) {
    for (size_t i{0}; i < 4; i++) {
      tree_points(("Point Light #" + std::to_string(i + 1)).c_str(), &stage.point_lights[i]);
    }
  }

  ImGui::End();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
#endif // __EDITOR_H__