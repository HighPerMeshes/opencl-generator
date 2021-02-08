#ifndef UnaryOperator_HPP
#define UnaryOperator_HPP


#include <ir/Printable.hpp>


namespace ct {

struct UnaryOperator {

  StringType op;
  Printable applied_to;
  bool postfix;

};

bool operator==(const UnaryOperator& lhs, const UnaryOperator& rhs) {
  return lhs.op == rhs.op && lhs.applied_to == rhs.applied_to && lhs.postfix == rhs.postfix;
}
bool operator!=(const UnaryOperator& lhs, const UnaryOperator& rhs) {
  return !(lhs == rhs);
}

template<typename Stream>
void print(Stream& stream, const UnaryOperator& unary) {
  if(!unary.postfix) {
    print(stream, unary.op);
  }
  print(stream, unary.applied_to);
  if(unary.postfix) {
    print(stream, unary.op);
  }
}

template <typename Visitor>
UnaryOperator transform(Visitor&& visitor, UnaryOperator&& unary) {
  return {
    visitor.visit(std::move(unary.op)), 
    visitor.visit(std::move(unary.applied_to)),
    visitor.visit(std::move(unary.postfix))
  };
}

const Type * get_type(const UnaryOperator& unary) {
  return get_type(unary.applied_to);
}



} // namespace ct

#endif /* UnaryOperator_HPP */
