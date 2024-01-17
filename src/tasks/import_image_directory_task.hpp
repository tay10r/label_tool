#pragma once

#include "../task.hpp"

#include <filesystem>
#include <vector>

class import_image_directory_task final : public task
{
public:
  explicit import_image_directory_task(const std::filesystem::path& p, int entries_per_step = 16);

  auto done() const -> bool override;

  auto get_name() const -> const char* override { return "Import Image Directory"; }

  auto take_frame_list() -> std::vector<std::filesystem::path>;

protected:
  void do_step() override;

private:
  std::vector<std::filesystem::path> m_frame_paths;

  std::filesystem::directory_iterator m_directory_iterator;

  bool m_done{ false };

  int m_entries_per_step{ 1 };
};
