//
// Copyright 2024 Debugging Life
//

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <limits>
#include <random>
#include <utility>
#include <vector>

static auto can_increment(int64_t value) -> bool {
  return value < std::numeric_limits<int64_t>::max() - 1;
}

template <std::regular T>
class instrumented {
  static int64_t c_eq;
  static int64_t c_cc;
  static int64_t c_mc;
  static int64_t c_ca;
  static int64_t c_ma;
  static int64_t c_sw;
  T value;

 public:
  explicit instrumented<T>(T value) : value(std::move(value)) {}
  ~instrumented<T>() = default;

  friend auto operator==(const instrumented& a, const instrumented& b) -> bool {
    if (can_increment(c_eq)) {
      c_eq++;
    }
    return a == b;
  }

  friend auto operator!=(const instrumented& a, const instrumented& b) -> bool {
    return !(a == b);
  }

  instrumented(const instrumented<T>& value) {
    if (can_increment(c_cc)) {
      c_cc++;
    }
    this->value = value.value;
  }

  instrumented(instrumented<T>&& value) noexcept {
    if (can_increment(c_mc)) {
      c_mc++;
    }
    static_cast<void>(std::exchange(this->value, value.value));
  }

  auto operator=(instrumented<T> copy) -> instrumented<T>& {
    if (can_increment(c_ca)) {
      c_ca++;
    }
    copy.swap(*this);
    return *this;
  }

  auto operator=(instrumented<T>&& copy) noexcept -> instrumented<T>& {
    if (can_increment(c_ma)) {
      c_ma++;
    }
    static_cast<void>(std::exchange(this->value, copy.value));
    return *this;
  }

  friend void swap(instrumented<T>& a, instrumented<T>& b) noexcept {
    a.swap(b);
  }

  void swap(instrumented<T>& rhs) noexcept {
    if (can_increment(c_sw)) {
      c_sw++;
    }
    using std::swap;
    swap(value, rhs.value);
  }

  static auto reset_count() -> void {
    c_eq = 0;
    c_cc = 0;
    c_mc = 0;
    c_ca = 0;
    c_ma = 0;
    c_sw = 0;
  }

  static auto print_count() -> void {
    std::cout << "c_eq" << c_eq << std::endl;
    std::cout << "c_cc" << c_cc << std::endl;
    std::cout << "c_mc" << c_mc << std::endl;
    std::cout << "c_ca" << c_ca << std::endl;
    std::cout << "c_ma" << c_ma << std::endl;
    std::cout << "c_sw" << c_sw << std::endl;
  }
};

template <std::regular T>
int64_t instrumented<T>::c_cc = 0;

template <std::regular T>
int64_t instrumented<T>::c_mc = 0;

template <std::regular T>
int64_t instrumented<T>::c_ca = 0;

template <std::regular T>
int64_t instrumented<T>::c_ma = 0;

template <std::regular T>
int64_t instrumented<T>::c_sw = 0;

template <std::regular T>
int64_t instrumented<T>::c_eq = 0;

auto main() -> int {
  std::vector<instrumented<double>> vec_d{};
  vec_d.reserve(2000);

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<double> dis(1.0, 2.0);

  std::generate_n(std::back_inserter(vec_d), 2000, [&dis, &gen] {
    return instrumented<double>(dis(gen));
  });

  instrumented<double>::print_count();
  instrumented<double>::reset_count();
}
