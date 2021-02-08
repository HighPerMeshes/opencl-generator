#ifndef BINARYOPERATOR_HPP
#define BINARYOPERATOR_HPP

#include <ir/Printable.hpp>

namespace ct
{

  struct BinaryOperator
  {

    Printable lhs;
    StringType op;
    Printable rhs;
  };

  bool operator==(const BinaryOperator &lhs, const BinaryOperator &rhs)
  {
    return lhs.lhs == rhs.lhs && lhs.op == rhs.op && lhs.rhs == rhs.rhs;
  }
  bool operator!=(const BinaryOperator &lhs, const BinaryOperator &rhs)
  {
    return !(lhs == rhs);
  }

  template <typename Stream>
  void print(Stream &stream, const BinaryOperator &binary)
  {
    print(stream, binary.lhs);
    stream << binary.op;
    print(stream, binary.rhs);
  }

  template <typename Visitor>
  BinaryOperator transform(Visitor &&visitor, BinaryOperator &&binary)
  {
    return {
        visitor.visit(std::move(binary.lhs)),
        visitor.visit(std::move(binary.op)),
        visitor.visit(std::move(binary.rhs))};
  }

  const Type * get_type(const BinaryOperator&) {
    return nullptr;
  }

} // namespace ct

#endif /* BINARYOPERATOR_HPP */
