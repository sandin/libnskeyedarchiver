#ifndef NSKEYEDARCHIVER_SCOPE_H
#define NSKEYEDARCHIVER_SCOPE_H

#include <type_traits>

namespace nskeyedarchiver {

template <typename Callable>
class scope_exit {
  Callable ExitFunction;
  bool Engaged = true;  // False once moved-from or release()d.

 public:
  template <typename Fp>
  explicit scope_exit(Fp&& F) : ExitFunction(std::forward<Fp>(F)) {}

  scope_exit(scope_exit&& Rhs) : ExitFunction(std::move(Rhs.ExitFunction)), Engaged(Rhs.Engaged) {
    Rhs.release();
  }
  scope_exit(const scope_exit&) = delete;
  scope_exit& operator=(scope_exit&&) = delete;
  scope_exit& operator=(const scope_exit&) = delete;

  void release() { Engaged = false; }

  ~scope_exit() {
    if (Engaged) ExitFunction();
  }
};

// Keeps the callable object that is passed in, and execute it at the
// destruction of the returned object (usually at the scope exit where the
// returned object is kept).
template <typename Callable>
scope_exit<typename std::decay<Callable>::type> make_scope_exit(Callable&& F) {
  return scope_exit<typename std::decay<Callable>::type>(std::forward<Callable>(F));
}

}  // namespace nskeyedarchiver

#endif  // NSKEYEDARCHIVER_SCOPE_H