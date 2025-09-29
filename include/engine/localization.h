#pragma once
#include <engine/interface.h>
#include <map>
#include <string>
class ILocalization : public Interface {
  std::string g_language;
  std::map<std::string, std::string> g_dictionary;

protected:
  const char* name() override {
    return "Localization";
  }
  void i_Init() override;
  void i_Quit() override;

public:
  void Load(std::string_view /*path*/);
  void Save(std::string_view /*path*/);
  std::string& GetString(const std::string& key) {
    if (g_dictionary.find(key) != g_dictionary.end()) {
      return g_dictionary[key];
    }
    return g_dictionary[""];
  }
  std::string& GetLanguage() {
    return g_language;
  }
  std::map<std::string, std::string>& GetDictionary() {
    return g_dictionary;
  }
};