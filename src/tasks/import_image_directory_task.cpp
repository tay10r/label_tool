#include "import_image_directory_task.hpp"

#include <algorithm>
#include <limits>

import_image_directory_task::import_image_directory_task(const std::filesystem::path& p, int entries_per_step)
  : task(std::numeric_limits<std::size_t>::max())
  , m_directory_iterator(p)
  , m_entries_per_step(entries_per_step)
{
}

auto
import_image_directory_task::done() const -> bool
{
  return m_done;
}

auto
import_image_directory_task::take_frame_list() -> std::vector<std::filesystem::path>
{
  return std::move(m_frame_paths);
}

void
import_image_directory_task::do_step()
{
  for (int i = 0; !done() && (i < m_entries_per_step); i++) {

    if (m_directory_iterator == std::filesystem::end(m_directory_iterator)) {
      std::sort(m_frame_paths.begin(), m_frame_paths.end());
      m_done = true;
      return;
    }

    std::filesystem::path p = m_directory_iterator->path();

    const std::string ext = p.extension().string();

    if ((ext == ".png") || (ext == ".bmp") || (ext == ".jpg") || (ext == ".jpeg")) {
      m_frame_paths.emplace_back(std::move(p));
    }

    m_directory_iterator++;
  }
}
