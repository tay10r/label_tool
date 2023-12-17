#pragma once

#include "bbox.hpp"

#include <filesystem>
#include <string>
#include <vector>

struct project
{
  struct class_def final
  {
    std::string name;

    int value{};
  };

  struct frame final
  {
    std::filesystem::path path;

    int class_id{};
  };

  std::filesystem::path image_directory_path{ "." };

  bbox<float, 2> crop_bbox{ { 0, 0 }, { 1, 1 } };

  std::vector<class_def> class_defs;

  std::vector<frame> frames;

  void load(const std::filesystem::path& p);

  void save(const std::filesystem::path& p) const;

  auto get_frame_class_id(int value) const -> const char*;

  auto get_or_create_frame(const std::string& path) -> frame*;
};
