#ifndef UNKNOWN_HPP
#define UNKNOWN_HPP


#include <ir/Printable.hpp>


#include <optional>

namespace ct {
struct Unknown {

  StringType message;

};

bool operator==(const Unknown& lhs, const Unknown& rhs) {
  return lhs.message == rhs.message;
}
bool operator!=(const Unknown& lhs, const Unknown& rhs) {
  return !(lhs == rhs);
}

template<typename Stream> 
void print(Stream& stream, const Unknown& unknown) {
  stream << "[[[ UNKNOWN " << unknown.message << " ]]]";
}

template<typename Visitor>
Unknown transform(Visitor&& visitor, Unknown&& unknown) {
  return {
    visitor.visit(std::move(unknown.message))
  };
}

const Type * get_type(const Unknown& expr) {
    return nullptr;
  }



} // namespace ct

#endif /* UNKNOWN_HPP */
