#include "histogram.hpp"

#include "deps/stb_image.h"

#include <algorithm>

auto
histogram::from_image(const std::uint8_t* data, std::int32_t w, std::int32_t h, const bbox<float, 2>& bounds)
  -> histogram
{
  histogram result;

  auto clamp = [](int x, int mx) -> int { return std::max(std::min(x, mx), 0); };

  const auto safe_bounds = bounds.empty() ? bbox<float, 2>{ { 0, 0 }, { 1, 1 } } : bounds;

  const int x0 = clamp(safe_bounds.min[0] * w, w - 1);
  const int x1 = clamp(safe_bounds.max[0] * w, w - 1);

  const int y0 = clamp((1 - safe_bounds.max[1]) * (h - 1), h - 1);
  const int y1 = clamp((1 - safe_bounds.min[1]) * (h - 1), h - 1);

  const float scale{ 1.0f / (static_cast<float>(x1 - x0) * static_cast<float>(y1 - y0)) };

  for (int y = y0; y < y1; y++) {
    for (int x = x0; x < x1; x++) {
      const int i = y * w + x;
      result.r[data[i * 3 + 0]] += scale;
      result.g[data[i * 3 + 1]] += scale;
      result.b[data[i * 3 + 2]] += scale;
    }
  }

  return result;
}

auto
histogram::from_image_path(const char* path, const bbox<float, 2>& bounds) -> histogram
{
  int w = 0;
  int h = 0;
  auto* data = stbi_load(path, &w, &h, nullptr, 3);
  if (data) {
    auto result = histogram::from_image(data, w, h, bounds);
    stbi_image_free(data);
    return result;
  }

  return {};
}
