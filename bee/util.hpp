#pragma once

#include "error.hpp"

#include <algorithm>
#include <set>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

namespace bee {

////////////////////////////////////////////////////////////////////////////////
// vector utils
//

template <class T, std::convertible_to<T> U>
void concat(std::vector<T>& dest, U&& item)
{
  dest.emplace_back(std::forward<U>(item));
}

template <class T, class U>
  requires std::convertible_to<typename std::decay_t<U>::value_type, T>
void concat(std::vector<T>& dest, U&& container)
{
  if constexpr (std::is_rvalue_reference_v<U>) {
    for (auto& item : container) { dest.emplace_back(std::move(item)); }
  } else {
    for (const auto& item : container) { dest.emplace_back(item); }
  }
}

template <class T> void concat_many(std::vector<T>&) {}

template <class T, class Y, class... Ts>
void concat_many(std::vector<T>& dest, Y&& source, Ts&&... tail)
{
  concat(dest, std::forward<Y>(source));
  concat_many(dest, std::forward<Ts>(tail)...);
}

template <class T>
concept is_container = std::is_same_v<std::vector<typename T::value_type>, T> ||
                       std::is_same_v<std::set<typename T::value_type>, T>;

template <class T> struct element_type;

template <is_container T> struct element_type<T> {
  using type = typename T::value_type;
};

template <class T> struct element_type {
  using type = T;
};

template <class T, class... Ts> auto compose_vector(T&& first, Ts&&... tail)
{
  std::vector<typename element_type<std::decay_t<T>>::type> output;
  concat_many(output, std::forward<T>(first), std::forward<Ts>(tail)...);
  return output;
}

template <class T, class... Ts> std::vector<T> compose_vector(Ts&&... tail)
{
  std::vector<T> output;
  concat_many(output, std::forward<Ts>(tail)...);
  return output;
}

template <class T, class... Ts> auto make_vec(T&& v, Ts&&... vs)
{
  return std::vector<std::decay_t<T>>{v, std::forward<Ts>(vs)...};
}

template <class T> auto to_vector(T&& container)
{
  return compose_vector(std::forward<T>(container));
}

template <class T, class Tuple> std::vector<T> tuple_to_vector(Tuple&& tuple)
{
  return std::apply(
    []<class... K>(K&&... elems) {
      return std::vector<T>{std::forward<K>(elems)...};
    },
    std::forward<Tuple>(tuple));
}

////////////////////////////////////////////////////////////////////////////////
// set utils
//

template <class T> void insert(std::set<T>& dest, const std::set<T>& source)
{
  for (const auto& item : source) { dest.insert(item); }
}

template <class T, class U> void insert(std::set<T>& dest, const U& item)
{
  dest.insert(T(item));
}

template <class T> void insert_many(__attribute__((unused)) std::set<T>& dest)
{}

template <class T, class Y, class... Ts>
void insert_many(std::set<T>& dest, const Y& source, const Ts&... tail)
{
  insert(dest, source);
  insert_many(dest, tail...);
}

template <class T, class... Ts>
std::set<T> compose_set(const std::set<T>& first, const Ts&... tail)
{
  std::set<T> output;
  insert_many(output, first, tail...);
  return output;
}

template <class T> auto to_set(const T& v)
{
  return std::set<typename T::value_type>(v.begin(), v.end());
}

template <class T, std::invocable<T> F> void delete_if(std::set<T>& m, F&& cond)
{
  for (auto it = m.begin(); it != m.end();) {
    auto it2 = it;
    it++;
    if (cond(*it2)) { m.erase(it2); }
  }
}

////////////////////////////////////////////////////////////////////////////////
// Iterator
//

template <class T> struct rev_it_t {
 public:
  rev_it_t(T& container) : _container(container) {}

  auto begin() { return _container.rbegin(); }
  auto end() { return _container.rend(); }

 private:
  T& _container;
};

template <class T> rev_it_t<T> rev_it(T& container)
{
  return rev_it_t(container);
}

////////////////////////////////////////////////////////////////////////////////
// Misc
//

template <class> constexpr bool always_false_v = false;

// template <class T> void sort_c(T& cont) { std::sort(cont.begin(),
// cont.end()); }

// template <class T, class F> void sort_c(T& cont, const F& compare)
// {
//   std::sort(cont.begin(), cont.end(), compare);
// }

template <typename... Us> struct is_one_of;

template <typename T, typename U, typename... Us>
struct is_one_of<T, U, Us...> {
 private:
  constexpr static bool _value()
  {
    if constexpr (std::is_same_v<T, U>)
      return true;
    else if constexpr ((sizeof...(Us)) > 0)
      return is_one_of<T, Us...>::value;
    else
      return false;
  }

 public:
  constexpr static bool value = _value();
};

template <typename... Us> constexpr bool is_one_of_v = is_one_of<Us...>::value;

} // namespace bee
