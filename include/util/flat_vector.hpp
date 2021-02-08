#ifndef FLAT_VECTOR_HPP
#define FLAT_VECTOR_HPP

#include <vector>
#include <type_traits>

namespace ct
{

template <typename T>
struct AlwaysFalse : std::false_type
{
};

template <typename T>
void flat_vector_push_back(std::vector<T> &vector, T &&t)
{
  vector.push_back(std::move(t));
}

template <typename T>
void flat_vector_push_back(std::vector<T> &vector, const T &t)
{
  vector.push_back(t);
}

template <typename T, typename Collection, std::enable_if_t<std::is_same_v<typename Collection::value_type, T>, int> = 0>
void flat_vector_push_back(std::vector<T> &vector, Collection &&collection)
{
  if constexpr (std::is_rvalue_reference_v<decltype(collection)>)
 {
    vector.insert(vector.end(), std::make_move_iterator(collection.begin()), std::make_move_iterator(collection.end()));
  }
  else
 {
    vector.insert(vector.end(), collection.begin(), collection.end());
  }
}

template <typename T, typename... Ts>
std::vector<T> flat_vector(Ts &&... ts)
{
  std::vector<T> res;

  (
      [&res](auto &&entry) {
        using Type = decltype(entry);

        if constexpr (std::is_same_v<std::decay_t<Type>, T>)
       {
          flat_vector_push_back(res, std::forward<Type>(entry));
        }
        else if constexpr (std::is_same_v<typename std::decay_t<Type>::value_type, T>)
       {
          flat_vector_push_back(res, std::forward<Type>(entry));
        }
        else
       {
          static_assert(AlwaysFalse<T>::value, "flat_vector: entry not a T or not a collection of Ts");
        }
      }(std::forward<Ts>(ts)),
      ...);

  return res;
}

} // namespace ct

#endif /* FLAT_VECTOR_HPP */
