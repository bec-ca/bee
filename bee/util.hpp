#pragma once

#include <algorithm>
#include <set>
#include <tuple>
#include <type_traits>
#include <vector>

namespace bee {

////////////////////////////////////////////////////////////////////////////////
// vector utils
//

template <class U, class T>
concept can_be_concatenated_to_vector =
  std::convertible_to<U, T> ||
  std::convertible_to<typename std::decay_t<U>::value_type, T>;

template <class T, std::convertible_to<T> U>
void concat(std::vector<T>& dest, U&& item)
{
  dest.emplace_back(std::forward<U>(item));
}

template <class T, class U>
  requires std::convertible_to<typename std::decay_t<U>::value_type, T>
void concat(std::vector<T>& dest, U&& container)
{
  if constexpr (std::is_rvalue_reference_v<U&&>) {
    for (auto&& item : container) { dest.emplace_back(std::move(item)); }
  } else {
    for (const auto& item : container) { dest.emplace_back(item); }
  }
}

template <class T> void concat_many(std::vector<T>&) {}

template <
  class T,
  can_be_concatenated_to_vector<T> Y,
  can_be_concatenated_to_vector<T>... Ts>
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

template <class T, class F> auto map_vector(const std::vector<T>& v, F&& f)
{
  std::vector<std::decay_t<std::invoke_result_t<F, T>>> output;
  for (const auto& item : v) { output.push_back(f(item)); }
  return output;
}

////////////////////////////////////////////////////////////////////////////////
// set utils
//

template <class T, class U>
  requires std::convertible_to<typename std::decay_t<U>::value_type, T>
void insert(std::set<T>& dest, U&& container)
{
  if constexpr (std::is_rvalue_reference_v<U&&>) {
    for (auto&& item : container) { dest.emplace(std::move(item)); }
  } else {
    for (const auto& item : container) { dest.emplace(item); }
  }
}

template <class T, class U> void insert(std::set<T>& dest, U&& item)
{
  dest.emplace(std::forward<U>(item));
}

template <class T> void insert_many(std::set<T>&) {}

template <class T, class Y, class... Ts>
void insert_many(std::set<T>& dest, Y&& source, Ts&&... tail)
{
  insert(dest, std::forward<Y>(source));
  insert_many(dest, std::forward<Ts>(tail)...);
}

template <class T, class... Ts> auto compose_set(Ts&&... tail)
{
  std::set<T> output;
  insert_many(output, std::forward<Ts>(tail)...);
  return output;
}

template <class T> auto to_set(T&& v)
{
  using U = typename std::decay_t<T>::value_type;
  std::set<U> out;
  insert(out, std::forward<T>(v));
  return out;
}

template <class T, std::invocable<T> F> void delete_if(std::set<T>& m, F&& cond)
{
  for (auto it = m.begin(); it != m.end();) {
    auto it2 = it;
    it++;
    if (cond(*it2)) { m.erase(it2); }
  }
}

template <class T, class F> auto map_set(const std::set<T>& v, F&& f)
{
  std::set<std::decay_t<std::invoke_result_t<F, T>>> output;
  for (const auto& item : v) { output.emplace(f(item)); }
  return output;
}

////////////////////////////////////////////////////////////////////////////////
// rev_it
//

namespace details {

template <class T> struct rev_it_t {
 public:
  rev_it_t(T& container) : _container(container) {}

  auto begin() { return _container.rbegin(); }
  auto end() { return _container.rend(); }

 private:
  T& _container;
};

} // namespace details

template <class T> details::rev_it_t<T> rev_it(T& container)
{
  return details::rev_it_t(container);
}

////////////////////////////////////////////////////////////////////////////////
// zip
//

namespace details {

template <class C1, class C2> struct ZipHelper {
 public:
  template <class I1, class I2> struct Iterator {
   public:
    using value_type =
      std::pair<typename I1::reference, typename I2::reference>;

    Iterator(const I1& it1, const I2& it2) : _it1(it1), _it2(it2) {}

    value_type operator*() const { return {*_it1, *_it2}; }

    Iterator& operator++()
    {
      _it1++;
      _it2++;
      return *this;
    }

    bool operator==(const Iterator& other) const
    {
      return _it1 == other._it1 || _it2 == other._it2;
    }

   private:
    I1 _it1;
    I2 _it2;
  };

  auto begin() { return Iterator{_c1.begin(), _c2.begin()}; }
  auto end() { return Iterator{_c1.end(), _c2.end()}; }

  ZipHelper(C1& c1, C2& c2) : _c1(c1), _c2(c2) {}

 private:
  C1& _c1;
  C2& _c2;
};

} // namespace details

template <class C1, class C2> auto zip(C1& c1, C2& c2)
{
  return details::ZipHelper{c1, c2};
}

} // namespace bee
