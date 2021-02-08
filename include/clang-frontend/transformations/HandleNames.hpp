#ifndef HANDLENAMES_HPP
#define HANDLENAMES_HPP

#include <ir.hpp>
#include <regex>

namespace ct
{

  auto HandleNames()
  {
    return Transformer{
        [](Variable &&variable) {
          std::regex not_a_valid_character("[^(0-9a-zA-Z_)]");
          variable.name.name = std::regex_replace(variable.name.name, not_a_valid_character, "_"); 
          return variable;
        }
    };
  }



} // namespace ct


#endif /* HANDLENAMES_HPP */
