#include "exporter.hpp"

#include "deps/stb_image.h"

#include <algorithm>

exporter::exporter(const project& prj)
  : m_project(prj)
{
}

void
exporter::process_next_frame()
{
  if (m_image_index >= m_project.frames.size()) {
    return;
  }

  if (m_image_index == 0) {
    prepare();
  }

  const auto& f = m_project.frames.at(m_image_index);

  int w = 0;
  int h = 0;

  auto* data = stbi_load(f.path.string().c_str(), &w, &h, nullptr, 3);
  if (!data) {
    m_image_index++;
    return;
  }

  crop_and_process_frame(f, data, w, h);

  stbi_image_free(data);

  m_image_index++;

  if (m_image_index == m_project.frames.size()) {
    finalize();
  }
}

void
exporter::crop_and_process_frame(const project::frame& f, const std::uint8_t* data, std::int32_t w, std::int32_t h)
{
  const int x0 = static_cast<int>(m_project.crop_bbox.min[0] * w);
  const int y0 = static_cast<int>((1.0 - m_project.crop_bbox.max[1]) * h);

  const int x1 = static_cast<int>(m_project.crop_bbox.max[0] * w);
  const int y1 = static_cast<int>((1.0 - m_project.crop_bbox.min[1]) * h);

  const int tmp_w = x1 - x0;
  const int tmp_h = y1 - y0;

  std::vector<std::uint8_t> tmp(tmp_w * tmp_h * 3, 0);

  for (int y = y0; y < y1; y++) {

    for (int x = x0; x < x1; x++) {

      const int dst_x = x - x0;
      const int dst_y = y - y0;

      auto* dst = &tmp[((dst_y * tmp_w) + dst_x) * 3];

      const auto* src = data + (y * w + x) * 3;

      dst[0] = src[0];
      dst[1] = src[1];
      dst[2] = src[2];
    }
  }

  process_frame(f, tmp.data(), tmp_w, tmp_h);
}
