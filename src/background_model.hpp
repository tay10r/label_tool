#pragma once

#include <array>
#include <filesystem>
#include <vector>

class task;

struct project;

class background_model final
{
public:
  void add_path(const std::filesystem::path& p);

  auto create_filter_task(const project& proj) const -> std::unique_ptr<task>;

private:
  std::vector<std::filesystem::path> m_image_paths;
};
