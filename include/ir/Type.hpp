#ifndef TYPE_HPP
#define TYPE_HPP

#include <ir/Printable.hpp>
#include <ir/Qualifier.hpp>
#include <ir/Name.hpp>
#include <util.hpp>

#include <vector>

namespace ct
{

  struct Type
  {
    bool is(const Qualifier &qualifier) const
    {
      using namespace std;
      return find(begin(qualifiers), end(qualifiers), qualifier) !=
             end(qualifiers);
    }

    Type with(Qualifier qualifier) const
    {
      Type result{*this};
      result.qualifiers.emplace_back(std::move(qualifier));
      return result;
    }

    Type with(const std::vector<Qualifier>& qualifiers) const
    {
      Type result{*this};
      result.qualifiers.insert(
        result.qualifiers.end(),
        qualifiers.begin(),
        qualifiers.end()  
      );
      return result;
    }

    Type without(Qualifier qualifier) const
    {
      std::vector<Qualifier> new_qualifiers;
      std::copy_if(qualifiers.begin(), qualifiers.end(), std::back_inserter(new_qualifiers),
                   [&qualifier](const auto &compareTo) { return qualifier != compareTo; });
      return {
          name,
          template_arguments,
          new_qualifiers,
      };
    }


    explicit Type(Name name) : name{std::move(name)}, template_arguments{}, qualifiers{} {}


    Type(Name name,
         std::vector<Printable> template_arguments,
         std::vector<Qualifier> qualifiers) : name{std::move(name)}, template_arguments{std::move(template_arguments)}, qualifiers{std::move(qualifiers)} {}

    Name name;
    std::vector<Printable> template_arguments;
    std::vector<Qualifier> qualifiers;

  }; // namespace ct

  bool operator==(const Type &lhs, const Type &rhs)
  {
    return lhs.name == rhs.name && vector_equals(lhs.template_arguments, rhs.template_arguments) && vector_equals(lhs.qualifiers, rhs.qualifiers);
  }
  bool operator!=(const Type &lhs, const Type &rhs)
  {
    return !(lhs == rhs);
  }

  template <typename Stream>
  void print(Stream &stream, const Type &type)
  {
    print(stream, type.name);
    if (!type.template_arguments.empty())
    {
      stream << "<";
      print_with_delimiter(stream, type.template_arguments);
      stream << ">";
    }
    if(not type.qualifiers.empty()) {
      stream << " ";
      print_with_delimiter(stream, type.qualifiers, " ");
    }
   }

  template <typename Visitor>
  Type transform(Visitor &&visitor, Type &&type)
  {
    return {
        visitor.visit(std::move(type.name)),
        visitor.visit(std::move(type.template_arguments)),
        visitor.visit(std::move(type.qualifiers))};
  }

  const ct::Type &get_template_type_parameter(const ct::Type &type, size_t index)
  {
    return ct::printable_get<ct::Type>(type.template_arguments.at(index), "get_template_type_parameter: template argument not a type");
  }

  ct::Type Int()
  {
    return ct::Type{ct::Name{"int"}};
  }

  ct::Type Float()
  {
    return ct::Type{ct::Name{"float"}};
  }

    ct::Type Float(size_t dim)
  {
    return ct::Type{ct::Name{std::string { "float" } + std::to_string(dim) }};
  }


  ct::Type SizeT()
  {
    return ct::Type{ct::Name{"size_t"}};
  }

  const Type * get_type(const Type& type) {
    return &type;
  }


} // namespace ct

#endif /* TYPE_HPP */
