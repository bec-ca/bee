#include <algorithm>

namespace bee {

template <class T> void sort(T& cont) { std::sort(cont.begin(), cont.end()); }

template <class T, class CMP> void sort(T& cont, CMP&& cmp)
{
  std::sort(cont.begin(), cont.end(), std::forward<CMP>(cmp));
}

} // namespace bee
