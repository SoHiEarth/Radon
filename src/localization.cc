#include <engine/debug.h>
#include <engine/localization.h>
#include <format>
#include <pugixml.hpp>

void ILocalization::IInit() {}

void ILocalization::IQuit() {}

void ILocalization::Load(std::string_view path) {
  pugi::xml_document doc;
  auto result = doc.load_file(path.data());
  if (!result) {
    engine_->GetDebug().Throw(std::format("Failed to load dictionary. {}, {}", path, result.description()));
  }
  pugi::xml_node language_node = doc.child("language");
  g_language_ = language_node.attribute("name").as_string();
  pugi::xml_node dictionary = doc.child("dictionary");
  ILocalization::g_dictionary_.clear();
  for (pugi::xml_node entry_node : dictionary.children("entry")) {
    ILocalization::g_dictionary_.insert(
        {entry_node.attribute("key").as_string(), entry_node.attribute("value").as_string()});
  }
}

void ILocalization::Save(std::string_view path) {
  pugi::xml_document doc;
  pugi::xml_node language = doc.append_child("language");
  language.append_attribute("name") = g_language_;
  pugi::xml_node dictionary = doc.append_child("dictionary");
  for (const auto& [key, value] : g_dictionary_) {
    pugi::xml_node entry_node = dictionary.append_child("entry");
    entry_node.append_attribute("key") = key;
    entry_node.append_attribute("value") = value;
  }
  bool save_result = doc.save_file(path.data());
  if (!save_result) {
    engine_->GetDebug().Throw(std::format("Failed to save level file. {}", path));
  }
}
