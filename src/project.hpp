#pragma once

#include "bbox.hpp"

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

/**
 * @brief This contains all the data related to a project and additionally contains data related to the UI
 * configuration.
 * */
struct project final
{
  /**
   * @brief Used to define a class.
   * */
  struct class_def final
  {
    /**
     * @brief The human-readable label for the class.
     * */
    std::string name;

    /**
     * @brief The value associated with the class.
     * */
    int value{};
  };

  /**
   * @brief Used to describe a labeled image.
   * */
  struct frame final
  {
    /**
     * @brief The path to the image.
     * */
    std::filesystem::path path;

    /**
     * @brief The ID associated with the frame (as a whole).
     * */
    std::optional<int> class_id{};

    int frame_id{};

    auto operator<(const frame& other) const -> bool { return path < other.path; }
  };

  struct widget final
  {
    std::string name;

    bool visible{ false };
  };

  bbox<float, 2> crop_bbox{ { 0, 0 }, { 1, 1 } };

  std::vector<class_def> class_defs;

  std::vector<frame> frames;

  std::vector<widget> widgets{
    widget{ "Help", true },         widget{ "Image List", false },   widget{ "Image View", false },
    { "Histogram", false },         { "Background Model", false },   { "Tasks", false },
    { "Class Definitions", false }, { "Class Distribution", false }, { "Notes", false }
  };

  int next_frame_id{ 0 };

  std::string notes;

  void load(const std::filesystem::path& p);

  void save(const std::filesystem::path& p) const;

  auto get_frame_class_id(int value) const -> const char*;

  auto get_or_create_frame(const std::string& path) -> frame*;

  void import_images(const std::vector<std::filesystem::path>& paths);

  void import_image_directory(const std::filesystem::path& p);

  void import_image(const std::filesystem::path& p, bool sort = true);
};
