#ifndef TRANSFORMER_HPP
#define TRANSFORMER_HPP

#include <type_traits>
#include <experimental/type_traits>

namespace ct
{

template<typename Op, typename... Args>
using call_invoke = decltype( std::declval<Op>()(std::declval<Args>()... ));

template <typename Visitor, typename T>
using transform_invoke =
    decltype(transform(std::declval<Visitor>(), std::declval<T>()));

template<typename Op>
struct Transformer {

  Op op;

  Transformer(Op op) : op { op } {}

  template<typename T2>
  std::decay_t<T2> visit(T2&& t) {
    
    auto result = std::forward<T2>(t);

    if constexpr (std::experimental::is_detected_v<call_invoke, decltype(op), decltype(std::move(result))>)
    {
      result = op(std::move(result));
    }
    if constexpr (std::experimental::is_detected_v<transform_invoke, decltype(*this), decltype(std::move(result))>)
    {
      result = transform(*this, std::move(result));
    }
    return result;  

  }

};

template<typename Op, typename Until>
struct TransformUntil {

  Op op;

  TransformUntil(Op op) : op { op } {}

  template<typename T2>
  std::decay_t<T2> visit(T2&& t) {
    auto result = std::forward<T2>(t);

    if constexpr (std::is_same_v<std::decay_t<T2>, Until>) {
      return result;
    }
    if constexpr (std::experimental::is_detected_v<call_invoke, decltype(op), decltype(std::move(result))>)
    {
      result = op(std::move(result));
    }
    if constexpr (std::experimental::is_detected_v<transform_invoke, decltype(*this), decltype(std::move(result))>)
    {
      result = transform(*this, std::move(result));
    }
    return result;  
  }

};

template<typename Until, typename Op>
auto MakeTransformUntil(Op op) {
  return TransformUntil<Op, Until> { op };
}


template<typename Op, typename Until>
struct TransformUntilDepthFirst {

  Op op;

  TransformUntilDepthFirst(Op op) : op { op } {}

  template<typename T2>
  std::decay_t<T2> visit(T2&& t) {
    auto result = std::forward<T2>(t);

    if constexpr (std::is_same_v<std::decay_t<T2>, Until>) {
      return result;
    }
    if constexpr (std::experimental::is_detected_v<transform_invoke, decltype(*this), decltype(std::move(result))>)
    {
      result = transform(*this, std::move(result));
    }
    if constexpr (std::experimental::is_detected_v<call_invoke, decltype(op), decltype(std::move(result))>)
    {
      result = op(std::move(result));
    }
    return result;  
  }

};

template<typename Until, typename Op>
auto MakeTransformUntilDepthFirst(Op op) {
  return TransformUntilDepthFirst<Op, Until> { op };
}

template<typename Op>
struct DepthFirstTransform {

  Op op;

  DepthFirstTransform(Op op) : op { op } {}

  template<typename T2>
  std::decay_t<T2> visit(T2&& t) {
    
    auto result = std::forward<T2>(t);

    if constexpr (std::experimental::is_detected_v<transform_invoke, decltype(*this), decltype(std::move(result))>)
    {
      result = transform(*this, std::move(result));
    }
    if constexpr (std::experimental::is_detected_v<call_invoke, decltype(op), decltype(std::move(result))>)
    {
      result = op(std::move(result));
    }
    return result;  

  }

};


} // namespace ct

#endif /* IRVISITOR_HPP */
