#ifndef ADDRESSOF_HPP
#define ADDRESSOF_HPP

#include <ir/Printable.hpp>
#include <ir/Type.hpp>

namespace ct
{

  struct AddressOf
  {
    Printable onExpression;

    AddressOf(const Printable& onExpression) : onExpression { onExpression }, type_ { get_type(onExpression)->with(Pointer()) } {}

    Type& type() {
      type_ = get_type(onExpression)->with(Pointer());
      return type_;
    }

    const Type& type() const {
      type_ = get_type(onExpression)->with(Pointer());
      return type_;
    }


    private:
      mutable Type type_;
  };

  bool operator==(const AddressOf &lhs, const AddressOf &rhs)
  {
    return lhs.onExpression == rhs.onExpression;
  }
  bool operator!=(const AddressOf &lhs, const AddressOf &rhs)
  {
    return !(lhs == rhs);
  }

  template <typename Stream>
  void print(Stream &stream, const AddressOf &subscript)
  {
    print(stream, "(&", subscript.onExpression, ")");
  }

  template <typename Visitor>
  AddressOf transform(Visitor &&visitor, AddressOf &&subscript)
  {
    return {
        visitor.visit(std::move(subscript.onExpression))
    };
  }

  const Type * get_type(const AddressOf& addressOf) {
    return &addressOf.type();
  }


} // namespace ct


#endif /* ADDRESSOF_HPP */
