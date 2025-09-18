module;
#include <functional>
#include <source_location>
#include <string>
#include <string_view>
export module metal.debug;

export namespace debug {
extern bool enable_trace_;
extern bool trace_source_file_;
extern bool trace_function_name_;
extern bool trace_line_number_;

void SetCallback(std::function<void(const char*, const char*, std::uint8_t)> callback) noexcept;
void Log(const char* msg,
         const std::source_location& location = std::source_location::current()) noexcept;
void Log(std::string_view msg,
         const std::source_location& location = std::source_location::current()) noexcept;
void Warning(const char* msg,
             const std::source_location& location = std::source_location::current()) noexcept;
void Warning(std::string_view msg,
             const std::source_location& location = std::source_location::current()) noexcept;
void Throw(const char* fmt, const std::source_location& location = std::source_location::current());
void Throw(std::string_view fmt,
           const std::source_location& location = std::source_location::current());
}  // namespace debug