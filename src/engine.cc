#include <engine/asset_manager.h>
#include <engine/audio.h>
#include <engine/debug.h>
#include <engine/devgui.h>
#include <engine/engine.h>
#include <engine/input.h>
#include <engine/io.h>
#include <engine/localization.h>
#include <engine/physics.h>
#include <engine/render.h>
#include <engine/telemetry.h>
#include <classes/level.h>

constexpr const char* kTimerInitName = "Engine Init";
constexpr const char* kTimerIoInitName = "IO Init";
constexpr const char* kTimerAssetManagerInitName = "Asset Manager Init";
constexpr const char* kTimerRenderInitName = "Render Init";
constexpr const char* kTimerDevguiInitName = "Dev GUI Init";
constexpr const char* kTimerInputInitName = "Input Init";
constexpr const char* kTimerAudioInitName = "Audio Init";
constexpr const char* kTimerLocalizationInitName = "Localization Init";
constexpr const char* kTimerPhysicsInitName = "Physics Init";
constexpr const char* kTimerInputUpdateName = "Input Update";
constexpr const char* kTimerLevelUpdateName = "Level Update";
constexpr const char* kTimerRenderUpdateName = "Render Update";
constexpr const char* kTimerPhysicsUpdateName = "Physics Update";
constexpr const char* kTimerLevelRenderName = "Level Render";
constexpr const char* kTimerRenderRenderName = "Render Render";
constexpr const char* kTimerAudioUpdateName = "Audio Update";
#define TIME(FUNC, NAME)                         \
  telemetry_->BeginTimer(NAME); \
  FUNC;                         \
  telemetry_->EndTimer(NAME);   \
  telemetry_->LogTimer(NAME);

Engine::Engine() {
  debug_ = new IDebug();
  telemetry_ = new ITelemetry();
  asset_manager_ = new IAssetManager();
  io_ = new IIO();
  localization_ = new ILocalization();
  input_ = new IInput();
  audio_ = new IAudio();
  physics_ = new IPhysics();
  renderer_ = new IRenderer();
  gui_ = new IGui();

  debug_->SetEngineInstance(this);
  telemetry_->SetEngineInstance(this);
  asset_manager_->SetEngineInstance(this);
  io_->SetEngineInstance(this);
  localization_->SetEngineInstance(this);
  input_->SetEngineInstance(this);
  audio_->SetEngineInstance(this);
  physics_->SetEngineInstance(this);
  renderer_->SetEngineInstance(this);
  gui_->SetEngineInstance(this);
}

Engine::~Engine() {
  delete gui_;
  delete renderer_;
  delete physics_;
  delete audio_;
  delete input_;
  delete localization_;
  delete io_;
  delete asset_manager_;
  delete telemetry_;
  delete debug_;
}

void Engine::Init() {
  telemetry_->Start();
  telemetry_->BeginTimer(kTimerInitName);
  TIME(io_->Start(), kTimerIoInitName);
  TIME(asset_manager_->Start(), kTimerAssetManagerInitName);
  TIME(renderer_->Start(), kTimerRenderInitName);
  TIME(gui_->Start(), kTimerDevguiInitName);
  TIME(input_->Start(), kTimerInputInitName);
  TIME(audio_->Start(), kTimerAudioInitName);
  TIME(localization_->Start(), kTimerLocalizationInitName);
  TIME(physics_->Start(), kTimerPhysicsInitName);
  telemetry_->EndTimer(kTimerInitName);
  telemetry_->LogTimer(kTimerInitName);
  telemetry_->UploadTimings(kEngineInitName, telemetry_->GetTimings());
}

void Engine::Update() {
  telemetry_->BeginFrame();
  TIME(input_->Update(), kTimerInputUpdateName);
  if (io_->GetLevel() != nullptr) {
    TIME(io_->GetLevel()->Update(), kTimerLevelUpdateName);
  }
  TIME(physics_->Update(), kTimerPhysicsUpdateName);
  TIME(renderer_->Update(), kTimerRenderUpdateName);
  TIME(audio_->Update(), kTimerAudioUpdateName);
}

void Engine::Render() {
  if (io_->GetLevel() != nullptr) {
    TIME(io_->GetLevel()->Render(), kTimerLevelRenderName);
  }
  TIME(renderer_->Render(), kTimerRenderRenderName);
}

void Engine::Quit() {
  if (io_->GetLevel() != nullptr) {
    io_->GetLevel()->Quit();
  }

  physics_->Stop();
  localization_->Stop();
  audio_->Stop();
  input_->Stop();
  gui_->Stop();
  renderer_->Stop();
  asset_manager_->Stop();
  io_->Stop();
  telemetry_->Stop();
}