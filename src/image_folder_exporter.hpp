#pragma once

#include "exporter.hpp"

#include <filesystem>

class image_folder_exporter final : public exporter
{
public:
  image_folder_exporter(const project& proj, const std::filesystem::path& output_path);

protected:
  void process_frame(const project::frame& f, const std::uint8_t* rgb, std::int32_t w, std::int32_t h) override;

  void prepare() override;

  void finalize() override;

private:
  std::filesystem::path m_output_path;

  int m_frame_counter{ 0 };
};
