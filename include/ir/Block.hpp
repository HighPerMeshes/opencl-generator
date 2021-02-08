#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <ir/Printable.hpp>
#include <ir/Statement.hpp>

#include <util.hpp>

namespace ct
{

  struct Block
  {

    std::vector<Statement> body;
  };

  bool operator==(const Block &lhs, const Block &rhs)
  {
    return vector_equals(lhs.body, rhs.body);
  }
  bool operator!=(const Block &lhs, const Block &rhs)
  {
    return !(lhs == rhs);
  }

  template <typename Stream>
  void print(Stream &stream, const Block &block)
  {
    print(stream, "{\n");
    print_with_delimiter(stream, block.body, "\n");
    print(stream, "\n}\n");
  }

  template <typename Visitor>
  Block transform(Visitor &&visitor, Block &&block)
  {
    return {
        visitor.visit(std::move(block.body)),
    };
  }

  const Type * get_type(const Block&) {
    return nullptr;
  }

} // namespace ct

#endif /* BLOCK_HPP */
