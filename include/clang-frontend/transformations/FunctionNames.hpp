#ifndef FUNCTIONNAMES_HPP
#define FUNCTIONNAMES_HPP

#include <ir.hpp>

namespace ct
{

  auto FunctionNames()
  {
    static size_t i = 0;

    return DepthFirstTransform{
        [](Function &&f) -> Function {
          f.name.name = std::string{"function_"} + std::to_string(i);
          i++;
          return f;
        }
    };
  }

  auto FunctionCallNames()
  {
    return Transformer{
        [](FunctionCall &&f) {
          if(f.function) {
            f.name = f.function.value().name;
          }
          return f;
        }
    };
  }


} // namespace ct

#endif /* DEQUALIFYNAMES_HPP */
