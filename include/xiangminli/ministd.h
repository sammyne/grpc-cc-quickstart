#pragma once

#include <memory>

namespace xiangminli::ministd {

using std::shared_ptr;
using std::unique_ptr;

template <class T, class Deleter>
shared_ptr<T> NewSharedPtr(T *ptr, Deleter d) {
  return shared_ptr<T>(ptr, [d](T *v) {
    if (v) {
      d(v);
    }
  });
}

template <class T, class Deleter>
unique_ptr<T, Deleter> NewUniquePtr(T *ptr, Deleter d) {
  // @dev unique_ptr will add the nil check
  return unique_ptr<T, Deleter>(ptr, d);
}

}  // namespace xiangminli::ministd
