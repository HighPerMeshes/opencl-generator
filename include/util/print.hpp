#ifndef PRINT_HPP
#define PRINT_HPP

#include <string>
#include <string_view>
#include <optional>

namespace ct
{
  
template<typename Stream>
void print(Stream& stream, size_t val) {
  stream << val;
}

template<typename Stream>
void print(Stream& stream, const std::string& s) {
  stream << s;
}

template<typename Stream>
void print(Stream& stream, const char * s) {
  stream << s;
}

template<typename Stream>
void print(Stream& stream, char c) {
  stream << c;
}

template<typename Stream>
void print(Stream& stream, const std::string_view& s) {
  stream << s;
}

template<typename Stream, typename T>
void print(Stream& stream, const std::optional<T>& optional) {
  if(optional) print(stream, optional.value());
}

template<typename Stream, typename... Args>
std::enable_if_t< (sizeof...(Args) > 1) , void>
print(Stream& stream, Args&&... args) {
  ( [](Stream& stream, auto&& arg) { print(stream, std::forward<decltype(arg)>(arg)); }(stream, std::forward<Args>(args)), ... );
}

} // namespace ct

#endif /* PRINT_HPP */
