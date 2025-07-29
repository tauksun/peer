#ifndef RETRY
#define RETRY

#include <utility>
template <typename Func, typename SuccessPredicate, typename... Args>

auto retry(Func func, SuccessPredicate isSuccess, int maxRetries,
           Args &&...args) -> decltype(func(std::forward<Args>(args)...)) {
  using RetType = decltype(func(std::forward<Args>(args)...));
  RetType result;

  int attempt = 0;
  while (attempt <= maxRetries) {
    result = func(std::forward<Args>(args)...);

    if (isSuccess(result)) {
      return result;
    }

    attempt++;
  }

  return result;
}

#endif
