module;
#include <map>
#include <string>
export module metal.localization;

export namespace localization {
extern std::string g_language;
extern std::map<std::string, std::string> g_dictionary;
void Init();
void Quit();
void Load(std::string_view path);
void Save(std::string_view path);
}  // namespace localization