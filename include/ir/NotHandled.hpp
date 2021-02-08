#ifndef NOTHANDLED_HPP
#define NOTHANDLED_HPP

#include <ir/Printable.hpp>

namespace ct
{

  struct NotHandled
  {
    Printable wraps;
  };

  bool operator==(const NotHandled &lhs, const NotHandled &rhs)
  {
    return lhs.wraps == rhs.wraps;
  }
  bool operator!=(const NotHandled &lhs, const NotHandled &rhs)
  {
    return !(lhs == rhs);
  }

  template <typename Stream>
  void print(Stream &stream, const NotHandled &not_handled)
  {
    print(stream, "[[NOT HANDLED: ", not_handled.wraps, "]]");
  }

  template <typename Visitor>
  NotHandled transform(Visitor &&visitor, NotHandled &&not_handled)
  {
    return {
        visitor.visit(std::move(not_handled.wraps)),
        };
  }

  const Type * get_type(const NotHandled& not_handled) {
    return get_type(not_handled.wraps);
  }

} // namespace ct


#endif /* NOTHANDLED_HPP */
