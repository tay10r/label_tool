#include "image_folder_exporter.hpp"

#include "deps/stb_image_write.h"

#include <fstream>
#include <iomanip>
#include <sstream>

image_folder_exporter::image_folder_exporter(const project& proj, const std::filesystem::path& output_path)
  : exporter(proj)
  , m_output_path(output_path)
{
}

void
image_folder_exporter::process_frame(const project::frame& f, const std::uint8_t* rgb, std::int32_t w, std::int32_t h)
{
  if (!f.class_id.has_value()) {
    return;
  }

  const char* class_id = get_project()->get_frame_class_id(f.class_id.value());

  const std::filesystem::path class_path = m_output_path / class_id;

  std::ostringstream name_stream;
  name_stream << std::setw(5) << std::setfill('0') << m_frame_counter++ << ".png";

  const std::filesystem::path frame_path = class_path / name_stream.str();

  stbi_write_png(frame_path.string().c_str(), w, h, 3, rgb, w * 3);
}

void
image_folder_exporter::prepare()
{
  for (const auto& def : get_project()->class_defs) {
    std::filesystem::create_directory(m_output_path / def.name);
  }
}

void
image_folder_exporter::finalize()
{
}
