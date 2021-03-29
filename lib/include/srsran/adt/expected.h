/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSRAN_EXPECTED_H
#define SRSRAN_EXPECTED_H

#include "srsran/common/srsran_assert.h"
#include <memory>
#include <system_error>

namespace srsran {

struct default_error_t {};

template <typename T, typename E>
class expected;

template <typename T>
struct is_expected : std::false_type {};
template <typename V, typename E>
struct is_expected<expected<V, E> > : std::true_type {};

template <typename T, typename E = default_error_t>
class expected
{
  static_assert(not std::is_same<T, E>::value, "Expected and unexpected types cannot be of the same type");

public:
  expected() : has_val(true), val(T{}) {}
  expected(T&& t) : has_val(true), val(std::forward<T>(t)) {}
  expected(const T& t) : has_val(true), val(t) {}
  expected(E&& e) : has_val(false), unexpected(std::forward<E>(e)) {}
  expected(const E& e) : has_val(false), unexpected(e) {}
  template <
      typename U,
      typename std::enable_if<std::is_convertible<U, T>::value and not is_expected<typename std::decay<U>::type>::value,
                              int>::type = 0>
  explicit expected(U&& u) : has_val(true), val(std::forward<U>(u))
  {}
  expected(const expected& other)
  {
    if (other.has_val) {
      construct_val(other.val);
    } else {
      construct_error(other.unexpected);
    }
  }
  expected(expected&& other) noexcept
  {
    if (other.has_val) {
      construct_val(std::move(other.val));
    } else {
      construct_error(std::move(other.unexpected));
    }
  }
  expected& operator=(const expected& other)
  {
    if (this != &other) {
      expected(other).swap(*this);
    }
    return *this;
  }
  expected& operator=(expected&& other) noexcept
  {
    expected(std::move(other)).swap(*this);
    return *this;
  }
  expected& operator=(const T& other) noexcept
  {
    if (not has_value()) {
      unexpected.~E();
      has_val = true;
    }
    val = other;
    return *this;
  }
  expected& operator=(T&& other) noexcept
  {
    if (not has_value()) {
      unexpected.~E();
      has_val = true;
    }
    val = std::move(other);
    return *this;
  }
  ~expected() { destroy(); }

  void set_error()
  {
    if (has_value()) {
      val.~T();
      construct_error(E{});
    } else {
      unexpected = E{};
    }
  }
  template <typename U>
  void set_error(U&& other)
  {
    if (has_value()) {
      val.~T();
      construct_error(std::forward<U>(other));
    } else {
      unexpected = std::forward<U>(other);
    }
  }

  explicit operator bool() const { return has_value(); }
  bool     has_value() const { return has_val; }
  bool     is_error() const { return not has_value(); }
  const T& value() const
  {
    srsran_assert(has_value(), "Bad expected<T> value access");
    return val;
  }
  T& value()
  {
    srsran_assert(has_value(), "Bad expected<T> value access");
    return val;
  }
  const E& error() const
  {
    srsran_assert(not has_value(), "Bad expected<T> error access");
    return unexpected;
  }
  E& error()
  {
    srsran_assert(not has_value(), "Bad expected<T> error access");
    return unexpected;
  }

  void swap(expected& other) noexcept
  {
    using std::swap;

    if (has_value() and other.has_value()) {
      swap(val, other.val);
    } else if (not has_value() and not other.has_value()) {
      swap(unexpected, other.unexpected);
    } else if (has_value() and not other.has_value()) {
      E err(std::move(other.unexpected));
      other.unexpected.~E();
      other.construct_val(std::move(val));
      val.~T();
      construct_error(std::move(err));
    } else if (!bool(*this) && bool(other)) {
      other.swap(*this);
    }
  }

private:
  void construct_val(const T& v) noexcept
  {
    has_val = true;
    new (&val) T(v);
  }
  void construct_val(T&& v) noexcept
  {
    has_val = true;
    new (&val) T(std::move(v));
  }
  void construct_error(const E& e) noexcept
  {
    has_val = false;
    new (&unexpected) E(e);
  }
  void construct_error(E&& e) noexcept
  {
    has_val = false;
    new (&unexpected) E(std::move(e));
  }
  void destroy()
  {
    if (has_value()) {
      val.~T();
    } else {
      unexpected.~E();
    }
  }

  bool has_val = false;
  union {
    T val;
    E unexpected;
  };
};

template <typename E>
using error_type = expected<std::true_type, E>;

} // namespace srsran

#endif // SRSRAN_EXPECTED_H
