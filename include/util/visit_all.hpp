#ifndef VISIT_ALL_HPP
#define VISIT_ALL_HPP

#include <utility>

template <typename On, typename... Visitors>
auto visit_all(
    On on,
    Visitors &&... visitors)
{
  (
      [](auto &&visitor, auto &on) { on = std::forward<decltype(visitor)>(visitor).visit(std::move(on)); }(std::forward<Visitors>(visitors), on), ...);
  return on;
}

#endif /* VISIT_ALL_HPP */
