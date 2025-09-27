#ifndef LOCALIZATION_H
#define LOCALIZATION_H
#include <map>
#include <string>
namespace localization {
extern std::string g_language;
extern std::map<std::string, std::string> g_dictionary;
void Init();
void Quit();
void Load(std::string_view /*path*/);
void Save(std::string_view /*path*/);
}  // namespace localization

#endif
