#pragma once
#include <engine/interface.h>

// Forward declarations only in header
class IAudio;
class IDebug;
class IGui;
class IInput;
class IIO;
class ILocalization;
class IPhysics;
class IRenderer;
class ITelemetry;
class IAssetManager;

class Engine {
private:
  // Engine OWNS the interfaces - no singletons!
  IAudio* audio_ = nullptr;
  IDebug* debug_ = nullptr;
  IGui* gui_ = nullptr;
  IInput* input_ = nullptr;
  IIO* io_ = nullptr;
  ILocalization* localization_ = nullptr;
  IPhysics* physics_ = nullptr;
  IRenderer* renderer_ = nullptr;
  ITelemetry* telemetry_ = nullptr;
  IAssetManager* asset_manager_ = nullptr;

public:
  Engine();
  ~Engine();

  // Delete copy/move to prevent multiple engines
  Engine(const Engine&) = delete;
  Engine& operator=(const Engine&) = delete;
  Engine(Engine&&) = delete;
  Engine& operator=(Engine&&) = delete;

  // Lifecycle
  void Init();
  void Update();
  void Render();
  void Quit();

  // Accessors - return references to owned instances
  IAudio& GetAudio() const {
    return *audio_;
  }
  IDebug& GetDebug() const {
    return *debug_;
  }
  IGui& GetGui() const {
    return *gui_;
  }
  IInput& GetInput() const {
    return *input_;
  }
  IIO& GetIO() const {
    return *io_;
  }
  ILocalization& GetLocalization() const {
    return *localization_;
  }
  IPhysics& GetPhysics() const {
    return *physics_;
  }
  IRenderer& GetRenderer() const {
    return *renderer_;
  }
  ITelemetry& GetTelemetry() const {
    return *telemetry_;
  }
  IAssetManager& GetAssetManager() const {
    return *asset_manager_;
  }
};