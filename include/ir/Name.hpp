#ifndef NAME_HPP
#define NAME_HPP

#include <ir/Printable.hpp>

namespace ct
{

  struct Name
  {
    StringType name;
  };

  bool operator==(const Name &lhs, const Name &rhs)
  {
    return lhs.name == rhs.name;
  }
  bool operator!=(const Name &lhs, const Name &rhs)
  {
    return !(lhs == rhs);
  }

  template <typename Stream>
  void print(Stream &stream, const Name &name) { stream << name.name; }

  template <typename Visitor>
  Name transform(Visitor &&visitor, Name &&name)
  {
    return {visitor.visit(std::move(name.name))};
  }

  const Type * get_type(const Name&) {
    return nullptr;
  }

} // namespace ct

#endif /* NAME_HPP */
