#ifndef STD_TRANSFORMS_HPP
#define STD_TRANSFORMS_HPP

#include <vector>
#include <optional>

namespace ct
{

template <typename Visitor, typename T>
std::vector<T> transform(Visitor &&visitor, std::vector<T> &&vec)
{
  for (auto &&value : vec)
  {
    value = visitor.visit(std::move(value));
  }
  return std::move(vec);
}

template <typename Visitor, typename T>
std::optional<T> transform(Visitor &&visitor, std::optional<T> &&optional)
{
  return optional ? std::optional<T>{visitor.visit(std::move(*optional))} : std::optional<T>{std::nullopt};
}

} // namespace ct

#endif /* STD_TRANSFORMS_HPP */
