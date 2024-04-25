#include "project.hpp"

#include <yaml-cpp/yaml.h>

#include <algorithm>
#include <fstream>
#include <iterator>

namespace {

template<typename T, int Dim>
auto
encode_bbox(const bbox<T, Dim>& b) -> YAML::Node
{
  YAML::Node node;
  node["min"] = b.min;
  node["max"] = b.max;
  return node;
}

template<typename T, int Dim>
void
decode_bbox(const YAML::Node& node, bbox<T, Dim>& b)
{
  {
    int i = 0;
    for (const auto& v : node["min"]) {
      if (i < Dim) {
        b.min[i] = v.as<T>();
      }
      i++;
    }
  }

  {
    int i = 0;
    for (const auto& v : node["max"]) {
      if (i < Dim) {
        b.max[i] = v.as<T>();
      }
      i++;
    }
  }
}

} // namespace

void
project::load(const std::filesystem::path& p)
{
  const auto root = YAML::LoadFile(p.string());

  next_frame_id = root["next_frame_id"].as<int>();

  decode_bbox(root["crop_bbox"], crop_bbox);

  for (const auto& class_def_node : root["class_defs"]) {
    class_def def;
    def.name = class_def_node["name"].as<std::string>();
    def.value = class_def_node["value"].as<int>();
    class_defs.emplace_back(std::move(def));
  }

  for (const auto& frame_node : root["frames"]) {

    frame f;

    if (frame_node["class_id"]) {
      f.class_id = frame_node["class_id"].as<int>();
    }

    f.frame_id = frame_node["id"].as<int>();

    f.path = frame_node["path"].as<std::string>();

    frames.emplace_back(std::move(f));
  }

  widgets.clear();

  for (const auto& widget_node : root["widgets"]) {

    widget w{ widget_node["name"].as<std::string>(), widget_node["visible"].as<bool>() };

    widgets.emplace_back(std::move(w));
  }

  notes = root["notes"].as<std::string>();
}

void
project::save(const std::filesystem::path& p) const
{
  YAML::Node root;

  root["next_frame_id"] = next_frame_id;

  root["crop_bbox"] = encode_bbox(crop_bbox);

  std::vector<YAML::Node> class_def_nodes;
  for (const auto& def : class_defs) {
    YAML::Node node;
    node["name"] = def.name;
    node["value"] = def.value;
    class_def_nodes.emplace_back(std::move(node));
  }
  root["class_defs"] = class_def_nodes;

  std::vector<YAML::Node> frame_nodes;
  for (const auto& f : frames) {

    YAML::Node node;

    node["path"] = f.path.string();

    if (f.class_id) {
      node["class_id"] = f.class_id.value();
    }

    node["id"] = f.frame_id;

    frame_nodes.emplace_back(std::move(node));
  }
  root["frames"] = frame_nodes;

  std::vector<YAML::Node> widget_nodes;
  for (const auto& w : widgets) {
    YAML::Node widget_node;
    widget_node["name"] = w.name;
    widget_node["visible"] = w.visible;
    widget_nodes.emplace_back(std::move(widget_node));
  }
  root["widgets"] = widget_nodes;

  root["notes"] = notes;

  std::ofstream file(p);
  file << root;
}

auto
project::get_frame_class_id(int value) const -> const char*
{
  for (const auto& def : class_defs) {
    if (def.value == value) {
      return def.name.c_str();
    }
  }

  return "";
}

auto
project::get_or_create_frame(const std::string& path) -> frame*
{
  auto it = std::lower_bound(
    frames.begin(), frames.end(), path, [](const auto& a, const auto& b) -> bool { return a.path < b; });

  if ((it == frames.end()) || (it->path != path)) {

    frames.emplace_back(frame{ path, 0 });

    it = std::next(frames.begin(), frames.size() - 1);
  }

  std::sort(frames.begin(), frames.end(), [](const auto& a, const auto& b) -> bool { return a.path < b.path; });

  return &(*it);
}

void
project::import_image(const std::filesystem::path& p, bool sort)
{
  auto it = std::lower_bound(
    frames.begin(), frames.end(), p, [](const frame& f, const std::filesystem::path& p) -> bool { return f.path < p; });

  if ((it == frames.end()) || (it->path != p)) {

    frames.emplace_back(frame{ p, /* class id */ std::nullopt, next_frame_id++ });

    if (sort) {
      std::sort(frames.begin(), frames.end());
    }
  }
}

void
project::import_images(const std::vector<std::filesystem::path>& paths)
{
  for (const auto& p : paths) {
    import_image(p, /* sort */ false);
  }

  std::sort(frames.begin(), frames.end());
}

void
project::import_image_directory(const std::filesystem::path& p)
{
  for (const auto& entry : std::filesystem::directory_iterator(p)) {
    const std::string ext = entry.path().extension().string();
    if ((ext == ".png") || (ext == ".bmp") || (ext == ".jpg")) {
      import_image(entry.path());
    }
  }
}
