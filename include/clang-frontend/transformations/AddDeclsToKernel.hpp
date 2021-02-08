#ifndef ADDDECLSTOKERNEL_HPP
#define ADDDECLSTOKERNEL_HPP

#include <ir.hpp>

namespace ct
{

template<typename Decls>
auto AddDeclsToKernel(Decls &arrayDeclarations)
{
  return Transformer{
      [&](Function &&kernel) -> Function {
        unique_vector(arrayDeclarations);

        if(!kernel.body) {
          return kernel;
        }

        arrayDeclarations.insert(arrayDeclarations.end(), kernel.body.value().body.begin(), kernel.body.value().body.end());

        return {
            std::move(kernel.return_type),
            std::move(kernel.name),
            std::move(kernel.parameters),
            Block { std::move(arrayDeclarations) }
          };
      }};
}

} // namespace ct

#endif /* ADDDECLSTOKERNEL_HPP */
