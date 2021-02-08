#ifndef DEQUALIFYNAMES_HPP
#define DEQUALIFYNAMES_HPP

#include <ir.hpp>

namespace ct
{

Name unqualify(const Name& name) {

  auto iter = std::find(
      name.name.rbegin(),
      name.name.rend(),
      ':'
  ).base();

  if(iter == name.name.begin()) {
      return name;
  }

  return {
        std::string {iter,
        name.name.end()
      }
  };
}

auto UnqualifyNames()
{
  return Transformer{ 
    []( Type&& name) {
      // auto old_name = name;
      // auto new_name = unqualify(old_name);
      return Int();
    }
   };
}

} // namespace ct

#endif /* DEQUALIFYNAMES_HPP */
