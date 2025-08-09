#include <engine/devgui.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <engine/global.h>

bool dev::hud_enabled = false;

void dev::Init() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows

  ImGui_ImplGlfw_InitForOpenGL(Engine::window.load(), true);
  ImGui_ImplOpenGL3_Init("#version 150");
}

void dev::Update() {
  if (!dev::hud_enabled) {
    // Hide mouse
    glfwSetInputMode(Engine::window.load(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    return;
  }
  // Show mouse
  glfwSetInputMode(Engine::window.load(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  ImGui::Begin("Dev HUD");
  ImGui::Text("Window Size: %d x %d", Engine::width.load(), Engine::height.load());
  ImGui::End();
}

void dev::LateUpdate() {
  if (!dev::hud_enabled) return;
  ImGui::Render();
  ImGui::EndFrame();
  ImGui::UpdatePlatformWindows();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void dev::Quit() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}
