#pragma once

#include <algorithm>
#include <array>

template<typename T, int Dim>
struct bbox final
{
  using vec_type = std::array<T, Dim>;

  vec_type min{};

  vec_type max{};

  void extend(const vec_type& p)
  {
    for (int i = 0; i < Dim; i++) {
      min[i] = std::min(min[i], p[i]);
      max[i] = std::max(max[i], p[i]);
    }
  }

  [[nodiscard]] auto empty() const -> bool
  {
    for (int i = 0; i < Dim; i++) {
      if (min[i] != max[i]) {
        return false;
      }
    }
    return true;
  }
};
