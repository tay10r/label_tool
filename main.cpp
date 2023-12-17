#include <uikit/main.hpp>

#include "image_viewer.hpp"
#include "project.hpp"

#include "custom_exporter.hpp"
#include "exporter.hpp"
#include "exporter_thread.hpp"

#include <imgui.h>

#include <imgui_stdlib.h>

#include <implot.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

struct labeled_entry final
{
  std::string path;

  int class_id{};

  int x{};
  int y{};

  int w{};
  int h{};
};

struct app_state final : public image_viewer_observer
{
  std::filesystem::path open_project_path;

  project open_project;

  std::vector<std::string> image_paths;

  std::vector<labeled_entry> labeled;

  std::vector<std::string> ignored;

  int selected_unlabeled{ -1 };

  std::unique_ptr<image_viewer> viewer{ image_viewer::create(this) };

  app_state(const std::filesystem::path& project_path)
    : open_project_path(project_path)
  {
    if (std::filesystem::exists(project_path)) {
      open_project.load(project_path);
    }

    scan_image_directory();
  }

  void save_state() { save_labeled(); }

protected:
  void on_frame_class_id_change(const std::string& path, int frame_class_id) override
  {
    open_project.get_or_create_frame(path)->class_id = frame_class_id;
  }

  void on_crop_bbox_change(const bbox<float, 2>& crop_bbox) override { open_project.crop_bbox = crop_bbox; }

  void save_labeled()
  {
    std::ofstream file("labeled.txt");

    for (const auto& ent : labeled) {
      file << ent.path;
      file << ' ';
      file << ent.class_id;
      file << ' ';
      file << ent.x;
      file << ' ';
      file << ent.y;
      file << ' ';
      file << ent.w;
      file << ' ';
      file << ent.h;
      file << std::endl;
    }
  }

  template<typename T>
  auto get_field(int line, std::istream& stream) -> T
  {
    std::string str;

    if (!std::getline(stream, str, ' ')) {
      std::ostringstream err;
      err << "Failed to read entry line " << line;
      throw std::runtime_error(err.str());
    }

    std::istringstream tmp(str);

    T value;

    tmp >> value;

    return value;
  }

  void load_entries(const std::filesystem::path& p)
  {
    std::ifstream file(p);

    int line_counter{ 1 };
    while (file) {

      std::string line;

      std::getline(file, line);

      std::istringstream line_stream(line);

      labeled_entry ent;

      ent.path = get_field<std::string>(line_counter, line_stream);
      ent.class_id = get_field<int>(line_counter, line_stream);
      ent.x = get_field<int>(line_counter, line_stream);
      ent.y = get_field<int>(line_counter, line_stream);
      ent.w = get_field<int>(line_counter, line_stream);
      ent.h = get_field<int>(line_counter, line_stream);

      line_counter++;

      labeled.emplace_back(std::move(ent));
    }
  }

  void scan_image_directory()
  {
    for (const auto& entry : std::filesystem::directory_iterator(open_project.image_directory_path)) {
      const auto path = std::filesystem::path(entry);
      if (path.extension() != ".png") {
        continue;
      }
      image_paths.emplace_back(path.string());
    }

    std::sort(image_paths.begin(), image_paths.end());
  }
};

class app_impl final : public uikit::app
{
public:
  void setup(uikit::platform& plt) override { plt.set_app_name("Label Tool"); }

  void teardown(uikit::platform& plt) override
  {
    // TODO : ask user about closing while exporting a dataset

    if (m_exporter_thread) {
      m_exporter_thread->cancel();
    }

    // TODO : ask user about quitting if there are unsaved changes, instead of just saving regardless

    m_state.open_project.save(m_state.open_project_path);
  }

  void loop(uikit::platform& plt) override
  {
    ImGui::DockSpaceOverViewport();

    render_main_menu_bar(plt);

    ImGui::SetNextWindowSize(ImVec2(256, 256), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Image List")) {
      render_image_list();
    }
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(256, 0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(256, 256), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Image Viewer")) {
      m_state.viewer->render(m_state.open_project);
    }
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(0, 256), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(256, 256), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Classes")) {
      render_class_list();
    }
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(256, 256), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(256, 256), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Class Distribution")) {
      render_class_distribution();
    }
    ImGui::End();
  }

protected:
  static void on_open_project(void* self_ptr, const char* project_path)
  {
    static_cast<app_impl*>(self_ptr)->open_project(project_path);
  }

  static void on_new_project(void* self_ptr, const char* project_directory_path)
  {
    auto* self = static_cast<app_impl*>(self_ptr);

    self->m_state = app_state(std::filesystem::path(project_directory_path) / "label_tool_project.yaml");
  }

  void open_project(const std::filesystem::path& p) { m_state = app_state(p); }

  void render_file_menu(uikit::platform& plt)
  {
    if (ImGui::Selectable("New Project")) {
      plt.open_directory_dialog("Select New Project Directory", this, on_new_project);
    }

    if (ImGui::Selectable("Open Project")) {
      plt.open_file_dialog("Open Label Tool Project", { "YAML Files", "*.yml *.yaml" }, this, on_open_project);
    }

    const auto disabled = !!m_exporter_thread;

    ImGui::BeginDisabled(disabled);

    if (ImGui::BeginMenu("Export")) {

      if (ImGui::Selectable("Builtin Format")) {
        m_exporter_thread.reset(new exporter_thread(std::make_unique<custom_exporter>(m_state.open_project)));
      }

      ImGui::EndMenu();
    }

    ImGui::EndDisabled();
  }

  void render_main_menu_bar(uikit::platform& plt)
  {
    if (!ImGui::BeginMainMenuBar()) {
      return;
    }

    if (ImGui::BeginMenu("File")) {
      render_file_menu(plt);
      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }

  void render_class_distribution()
  {
    if (!ImPlot::BeginPlot("Class Distribution")) {
      return;
    }

    float counts[3]{ 1, 0.1, 2 };

    ImPlot::PlotBars("Distribution", counts, 3);

    ImPlot::EndPlot();
  }

  void render_class_list()
  {
    std::vector<std::size_t> to_delete;

    for (std::size_t i = 0; i < m_state.open_project.class_defs.size(); i++) {
      ImGui::PushID(static_cast<int>(i));

      auto& def = m_state.open_project.class_defs.at(i);
      ImGui::InputText("Name", &def.name);
      ImGui::InputInt("Value", &def.value);

      if (ImGui::Button("Delete")) {
        to_delete.emplace_back(i);
      }

      ImGui::PopID();

      ImGui::Separator();
    }

    if (ImGui::Button("Add")) {
      m_state.open_project.class_defs.emplace_back(project::class_def{ "Unknown", 0 });
    }

    std::size_t counter = 0;
    for (auto idx : to_delete) {
      m_state.open_project.class_defs.erase(std::next(m_state.open_project.class_defs.begin(), idx - counter));
      counter++;
    }
  }

  void render_image_list()
  {
    if (ImGui::BeginTabBar("Image List Tabs")) {
      if (ImGui::BeginTabItem("Unlabeled")) {
        render_unlabeled_image_list();
        ImGui::EndTabItem();
      }
      if (ImGui::BeginTabItem("Labeled")) {
        ImGui::EndTabItem();
      }
      if (ImGui::BeginTabItem("Ignored")) {
        ImGui::EndTabItem();
      }
      ImGui::EndTabBar();
    }
  }

  void render_unlabeled_image_list()
  {
    ImGui::BeginChild("Unlabeled Image List");

    for (std::size_t i = 0; i < m_state.image_paths.size(); i++) {
      const auto selected = static_cast<int>(i) == m_state.selected_unlabeled;

      const auto& path = m_state.image_paths[i];

      if (ImGui::Selectable(path.c_str(), selected)) {
        m_state.selected_unlabeled = i;
        on_image_selection(path);
      }
    }

    ImGui::EndChild();
  }

  void on_image_selection(const std::string& path)
  {
    m_state.viewer->open_image(path.c_str(), m_state.open_project.get_or_create_frame(path)->class_id);
  }

private:
  app_state m_state{ "label_tool_project.yaml" };

  std::unique_ptr<exporter_thread> m_exporter_thread;
};

} // namespace

namespace uikit {

auto
app::create() -> std::unique_ptr<app>
{
  return std::make_unique<app_impl>();
}

} // namespace uikit
