#ifndef RETURN_HPP
#define RETURN_HPP

#include <ir/Printable.hpp>

#include <optional>

namespace ct
{

  struct Return
  {

    std::optional<Printable> returnValue;
  };

  bool operator==(const Return &lhs, const Return &rhs)
  {
    return lhs.returnValue == rhs.returnValue;
  }
  bool operator!=(const Return &lhs, const Return &rhs)
  {
    return !(lhs == rhs);
  }

  template <typename Stream>
  void print(Stream &stream, const Return &return_instance)
  {
    stream << "return";
    if (return_instance.returnValue)
    {
      stream << " ";
      print(stream, return_instance.returnValue.value());
    }
  }

  template <typename Visitor>
  Return transform(Visitor &&visitor, Return &&return_instance)
  {
    return {visitor.visit(std::move(return_instance.returnValue))};
  }

  const Type * get_type(const Return&) {
    return nullptr;
  }

} // namespace ct

#endif /* RETURN_HPP */
