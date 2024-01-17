#pragma once

#include "project.hpp"

#include <filesystem>
#include <vector>

class exporter
{
public:
  exporter(const project& prj);

  virtual ~exporter() = default;

  auto get_remaining_frames() const -> std::size_t { return m_project.frames.size() - m_image_index; }

  void process_next_frame();

protected:
  void crop_and_process_frame(const project::frame& f, const std::uint8_t* data, std::int32_t w, std::int32_t h);

  virtual void prepare() = 0;

  virtual void finalize() = 0;

  virtual void process_frame(const project::frame& f, const std::uint8_t* rgb, std::int32_t w, std::int32_t h) = 0;

  virtual auto get_project() const -> const project* { return &m_project; }

  virtual auto get_image_index() const -> std::size_t { return m_image_index; }

  void move_to_next_frame();

private:
  project m_project;

  std::size_t m_image_index{};
};
