#pragma once
#include <engine/interface.h>
#include <map>
#include <string>
class ILocalization : public Interface {
  std::string g_language_;
  std::map<std::string, std::string> g_dictionary_;

protected:
  void IInit() override;
  void IQuit() override;

public:
  const char* Name() override {
    return "Localization";
  }
  void Load(std::string_view /*path*/);
  void Save(std::string_view /*path*/);
  std::string& GetString(const std::string& key) {
    if (g_dictionary_.contains(key)) {
      return g_dictionary_[key];
    }
    return g_dictionary_[""];
  }
  std::string& GetLanguage() {
    return g_language_;
  }
  std::map<std::string, std::string>& GetDictionary() {
    return g_dictionary_;
  }
};