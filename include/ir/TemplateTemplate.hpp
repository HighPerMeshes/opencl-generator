#ifndef TEMPLATETEMPALTE_HPP
#define TEMPLATETEMPALTE_HPP

#include <ir/Name.hpp>

namespace ct
{

  struct TemplateTemplate
  {

    Name name;
  };

  bool operator==(const TemplateTemplate &lhs, const TemplateTemplate &rhs)
  {
    return lhs.name == rhs.name;
  }
  bool operator!=(const TemplateTemplate &lhs, const TemplateTemplate &rhs)
  {
    return !(lhs == rhs);
  }

  template <typename Stream>
  void print(Stream &stream, const TemplateTemplate &templateTemplate)
  {
    print(stream, templateTemplate.name);
  }

  template <typename Visitor>
  TemplateTemplate transform(Visitor &&visitor, Visitor &&templateTemplate)
  {
    return {visitor.visit(std::move(templateTemplate.name))};
  }

  const Type * get_type(const TemplateTemplate&) {
    return nullptr;
  }


} // namespace ct

#endif /* TEMPLATETEMPALTE_HPP */
