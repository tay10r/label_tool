#pragma once

#include "bbox.hpp"

#include <array>

#include <cstdint>

struct histogram final
{
  using channel_data_type = std::array<float, 256>;

  channel_data_type r{};

  channel_data_type g{};

  channel_data_type b{};

  static auto from_image(const std::uint8_t* data, std::int32_t w, std::int32_t h, const bbox<float, 2>& bounds)
    -> histogram;

  static auto from_image_path(const char* path, const bbox<float, 2>& bounds) -> histogram;
};
