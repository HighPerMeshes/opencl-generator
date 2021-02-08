#ifndef POLYMORHICVALUE_HPP
#define POLYMORHICVALUE_HPP

#include <functional>
#include <memory>
#include <optional>
#include <type_traits>
#include <variant>
#include <iostream>

namespace ct
{

auto Default()
{
  return [](auto &&) { return; };
}
template <typename T>
auto Default(T &&t)
{
  return [&t](auto &&) { return std::forward<T>(t); };
}

template <class... Ts>
struct overloaded : Ts...
{
  using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...)->overloaded<Ts...>;

template<typename Variant, typename T, typename... Args> auto make_variant(Args&&... args) {
    return Variant { std::in_place_type_t<T> {}, std::forward<Args>(args)... };
} 

template <typename Op, typename... Ops, typename... Args>
auto match(std::variant<Args...>&& variant, Op &&op, Ops &&... ops)
{
  return std::visit(
      overloaded{
          std::forward<Op>(op),
          std::forward<Ops>(ops)...},
      variant);
}

template <typename Op, typename... Ops, typename... Args>
auto match(std::variant<Args...>& variant, Op &&op, Ops &&... ops)
{
  return std::visit(
      overloaded{
          std::forward<Op>(op),
          std::forward<Ops>(ops)...},
      variant);
}

template <typename Op, typename... Ops, typename... Args>
auto match(const std::variant<Args...>& variant, Op &&op, Ops &&... ops)
{
  return std::visit(
      overloaded{
          std::forward<Op>(op),
          std::forward<Ops>(ops)...},
      variant);
}

template<typename T, typename... Args>
auto make_unique(Args&&... args) -> std::unique_ptr<T> {
    if constexpr (std::is_constructible_v<T, Args...>)
        return std::make_unique<T>(std::forward<Args>(args)...);
    else
        return std::unique_ptr<T>(new T{std::forward<Args>(args)...});
}

template <typename T> struct Term {

  template<typename... Args> Term(Args&&... args) : ptr { ct::make_unique<T>(std::forward<Args>(args)...) } {}
  Term(const Term& other) : ptr { std::make_unique<T>( *other.ptr ) } {}
  Term(Term& other) : Term { static_cast<const Term&>(other) } {} 
  Term(Term&& other) : ptr { std::move(other.ptr) } {}
  Term& operator=(const Term& other) {
    return *this = Term { other }; 
  }
  Term& operator=(Term&& other) {
    ptr = std::move(other.ptr);
    return *this;
  }; 

  T& get() { return *ptr; }
  const T& get() const { return *ptr; }

private:
  std::unique_ptr<T> ptr;
};

template<typename T>
bool operator==(const Term<T>& lhs, const Term<T>& rhs) {
  return lhs.get() == rhs.get();
}
template<typename T>
bool operator!=(const Term<T>& lhs, const Term<T>& rhs) {
  return !(lhs == rhs);
}



} // namespace ct

#endif /* POLYMORHICVALUE_HPP */
