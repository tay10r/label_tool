#include "custom_exporter.hpp"

#include <nlohmann/json.hpp>

#include "deps/stb_image_write.h"

#include <fstream>

#include <cmath>

namespace {

auto
generate_path(const std::size_t idx, std::size_t num_images)
{
  std::ostringstream stream;
  stream << std::setw(static_cast<int>(std::log10(num_images) + 1)) << std::setfill('0') << idx;
  stream << ".png";
  return stream.str();
}

} // namespace

void
custom_exporter::process_frame(const project::frame& f, const std::uint8_t* rgb, std::int32_t w, std::int32_t h)
{
  if (!f.class_id.has_value()) {
    return;
  }

  const auto path = generate_path(m_output_entries.size(), get_project()->frames.size());

  stbi_write_png(path.c_str(), w, h, 3, rgb, w * 3);

  output_entry entry;

  entry.class_id = f.class_id.value();

  m_output_entries.emplace_back(std::move(entry));
}

void
custom_exporter::prepare()
{
}

void
custom_exporter::finalize()
{
  nlohmann::json root;

  std::vector<nlohmann::json> entry_nodes;

  for (const auto& entry : m_output_entries) {

    nlohmann::json entry_node;

    entry_node["class_id"] = entry.class_id;

    entry_nodes.emplace_back(std::move(entry_node));
  }

  root["entries"] = entry_nodes;

  std::ofstream file("entries.json");

  file << root;
}
