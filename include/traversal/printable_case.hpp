#ifndef PRINTABLE_CASE_HPP
#define PRINTABLE_CASE_HPP

namespace ct
{

namespace internal
{

template <typename Op>
struct MatchPrintable
{
  Op op;

  MatchPrintable(Op op) : op{op} {}

  template <typename T,
            typename = std::enable_if_t<std::is_invocable_v<Op, T &&>>>
  auto operator()(Term<T> &term) -> Printable
  {
    return make_printable(op(std::move(term.get())));
  }

  template <typename T>
  auto operator()(T &&t) -> Printable { return make_printable(std::forward<T>(t).get()); }
};
} // namespace internal

template <typename Op>
auto printable_case(Op op)
{
  return [op](Printable &&printable) {
    return match(
        printable,
        internal::MatchPrintable{op});
  };
}

} // namespace ct

#endif /* PRINTABLE_CASE_HPP */
