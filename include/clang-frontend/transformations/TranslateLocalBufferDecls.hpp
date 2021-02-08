#ifndef TRANSLATELOCALBUFFERDECLS_HPP
#define TRANSLATELOCALBUFFERDECLS_HPP

#include <ir.hpp>

namespace ct
{

auto TranslateLocalBufferDecls()
{
  return Transformer{
      printable_case(
          [](VariableDeclaration &&decl) {
            if (decl.variable.type.name.name != "HPM::internal::LocalBuffer")
              return decl;

            auto &buffer = printable_get<Type>(
                decl.variable.type.template_arguments[0],
                "TranslateLocalBufferDecls: buffer not found");

            auto &pointer_type = printable_get<Type>(
                buffer.template_arguments[0],
                "TranslateLocalBufferDecls: type not found");

            return VariableDeclaration{
                Variable{
                    pointer_type.with(Global()).with(Pointer()),
                    decl.variable.name},
                decl.definition};
          })};
}
} // namespace ct

#endif /* TRANSLATELOCALBUFFERDECLS_HPP */
