#include <engine/debug.h>
#include <engine/localization.h>
#include <format>
#include <pugixml.hpp>

std::string localization::g_language;
std::map<std::string, std::string> localization::g_dictionary{};

void localization::Init() {
  debug::Log("Initialized localization");
}

void localization::Quit() {
  debug::Log("Quit localization");
}

void localization::Load(std::string_view path) {
  pugi::xml_document doc;
  auto result = doc.load_file(path.data());
  if (!result) {
    debug::Throw(std::format("Failed to load dictionary. {}, {}", path, result.description()));
  }
  pugi::xml_node language_node = doc.child("language");
  g_language = language_node.attribute("name").as_string();
  pugi::xml_node dictionary = doc.child("dictionary");
  localization::g_dictionary.clear();
  for (pugi::xml_node entry_node : dictionary.children("entry")) {
    localization::g_dictionary.insert(
        {entry_node.attribute("key").as_string(), entry_node.attribute("value").as_string()});
  }
}

void localization::Save(std::string_view path) {
  pugi::xml_document doc;
  pugi::xml_node language = doc.append_child("language");
  language.append_attribute("name") = g_language;
  pugi::xml_node dictionary = doc.append_child("dictionary");
  for (const auto& [key, value] : localization::g_dictionary) {
    pugi::xml_node entry_node = dictionary.append_child("entry");
    entry_node.append_attribute("key") = key;
    entry_node.append_attribute("value") = value;
  }
  bool save_result = doc.save_file(path.data());
  if (!save_result) {
    debug::Throw(std::format("Failed to save level file. {}", path));
  }
}
