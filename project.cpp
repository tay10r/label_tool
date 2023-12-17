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
  const auto root = YAML::LoadFile(p);

  image_directory_path = root["image_directory"].as<std::string>();

  decode_bbox(root["crop_bbox"], crop_bbox);

  for (const auto& class_def_node : root["class_defs"]) {
    class_def def;
    def.name = class_def_node["name"].as<std::string>();
    def.value = class_def_node["value"].as<int>();
    class_defs.emplace_back(std::move(def));
  }

  for (const auto& frame_node : root["frames"]) {
    frame f;
    f.class_id = frame_node["class_id"].as<int>();
    f.path = frame_node["path"].as<std::string>();
    frames.emplace_back(std::move(f));
  }
}

void
project::save(const std::filesystem::path& p) const
{
  YAML::Node root;

  root["image_directory"] = image_directory_path.string();

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
    node["class_id"] = f.class_id;
    frame_nodes.emplace_back(std::move(node));
  }
  root["frames"] = frame_nodes;

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
