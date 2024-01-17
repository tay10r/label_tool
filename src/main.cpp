#include <uikit/main.hpp>

#include "project.hpp"

#include "custom_exporter.hpp"
#include "edit_moderator.hpp"
#include "exporter.hpp"
#include "exporter_thread.hpp"
#include "task_manager.hpp"
#include "widget_manager.hpp"

#include "widgets/background_model_widget.hpp"
#include "widgets/class_def_widget.hpp"
#include "widgets/class_dist_widget.hpp"
#include "widgets/help_widget.hpp"
#include "widgets/histogram_widget.hpp"
#include "widgets/image_list_widget.hpp"
#include "widgets/image_view_widget.hpp"
#include "widgets/notes_widget.hpp"
#include "widgets/tasks_widget.hpp"

#include "tasks/import_image_directory_task.hpp"

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

struct app_state final : public image_view_observer
{
  std::filesystem::path project_path;

  edit_moderator<project> proj;

  widget_manager widget_man;

  std::vector<std::string> image_paths;

  std::vector<labeled_entry> labeled;

  std::vector<std::string> ignored;

  int selected_unlabeled{ -1 };

  std::unique_ptr<histogram_widget> histogram_viewer{ histogram_widget::create() };

  app_state(const std::filesystem::path& project_path, task_manager* task_mgr)
    : project_path(project_path)
  {
    widget_man.add_widget("Help", std::make_unique<help_widget>(), true);
    widget_man.add_widget("Image List", std::make_unique<image_list_widget>(this, on_image_select), false);
    widget_man.add_widget("Image View", image_view_widget::create(this), false);
    widget_man.add_widget("Histogram", histogram_widget::create(), false);
    widget_man.add_widget("Background Model", background_model_widget::create(task_mgr), false);
    widget_man.add_widget("Tasks", std::make_unique<tasks_widget>(task_mgr), false);
    widget_man.add_widget("Class Definitions", std::make_unique<class_def_widget>(), false);
    widget_man.add_widget("Class Distribution", std::make_unique<class_dist_widget>(), false);
    widget_man.add_widget("Notes", std::make_unique<notes_widget>(), false);

    if (std::filesystem::exists(project_path)) {
      project p;
      p.load(project_path);
      proj = edit_moderator<project>(std::move(p));
    }
  }

  void save_state() { save_labeled(); }

protected:
  static void on_image_select(void* self_ptr, std::size_t index)
  {
    auto* self = static_cast<app_state*>(self_ptr);

    self->widget_man.on_image_select(self->proj.get(), index);
  }

  void on_frame_class_id_change(const std::string& path, int frame_class_id) override
  {
    proj.edit().get_or_create_frame(path)->class_id = frame_class_id;
  }

  void on_crop_bbox_change(const bbox<float, 2>& crop_bbox) override { proj.edit().crop_bbox = crop_bbox; }

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

    if (m_state.proj.has_unsaved_changes()) {
      // TODO : warn user
    }

    m_state.proj.get().save(m_state.project_path);

    m_task_manager->cancel_all();
  }

  void loop(uikit::platform& plt) override
  {
    m_task_manager->poll();

    ImGui::DockSpaceOverViewport();

    if (m_state.proj.changed()) {

      m_state.widget_man.on_project_change(m_state.proj.get());

      m_state.proj.ack_change();
    }

    render_main_menu_bar(plt);

    m_state.widget_man.render_widgets(m_state.proj);
  }

protected:
  static void on_open_project(void* self_ptr, const char* project_path)
  {
    static_cast<app_impl*>(self_ptr)->open_project(project_path);
  }

  static void on_new_project(void* self_ptr, const char* project_directory_path)
  {
    auto* self = static_cast<app_impl*>(self_ptr);

    self->m_state =
      app_state(std::filesystem::path(project_directory_path) / "label_tool_project.yaml", self->m_task_manager.get());
  }

  static void on_image_directory_import_complete(void* self_ptr, int task_id, task* t, bool success)
  {
    if (!success) {
      return;
    }

    auto* self = static_cast<app_impl*>(self_ptr);

    auto* derived_t = static_cast<import_image_directory_task*>(t);

    self->m_state.proj.edit().import_images(derived_t->take_frame_list());
  }

  static void on_image_directory_import(void* self_ptr, const char* directory_path)
  {
    auto* self = static_cast<app_impl*>(self_ptr);

    auto t = std::make_unique<import_image_directory_task>(directory_path);

    self->m_task_manager->queue(std::move(t), self, on_image_directory_import_complete);
  }

  void save_project(const std::filesystem::path& project_path)
  {
    //
  }

  void save_project()
  {
    if (m_state.project_path.empty()) {
      return;
    }

    save_project(m_state.project_path);
  }

  void open_project(const std::filesystem::path& p) { m_state = app_state(p, m_task_manager.get()); }

  void render_file_menu(uikit::platform& plt)
  {
    if (ImGui::Selectable("New Project")) {
      plt.open_directory_dialog("Select New Project Directory", this, on_new_project);
    }

    if (ImGui::Selectable("Open Project")) {
      plt.open_file_dialog("Open Label Tool Project", { "YAML Files", "*.yml *.yaml" }, this, on_open_project);
    }

    if (ImGui::Selectable("Import Image Directory")) {
      plt.open_directory_dialog("Open Image Directory", this, on_image_directory_import);
    }

    const auto disabled = !!m_exporter_thread;

    ImGui::BeginDisabled(disabled);

    if (ImGui::BeginMenu("Export")) {

      if (ImGui::Selectable("Builtin Format")) {
        m_exporter_thread.reset(new exporter_thread(std::make_unique<custom_exporter>(m_state.proj.get())));
      }

      ImGui::EndMenu();
    }

    ImGui::EndDisabled();
  }

  void render_edit_menu()
  {
    if (ImGui::Selectable("Undo")) {
      m_state.proj.undo();
    }

    if (ImGui::Selectable("Redo")) {
      m_state.proj.redo();
    }
  }

  void render_view_menu() { m_state.widget_man.render_visibility_menu(m_state.proj); }

  void render_main_menu_bar(uikit::platform& plt)
  {
    if (!ImGui::BeginMainMenuBar()) {
      return;
    }

    if (ImGui::BeginMenu("File")) {
      render_file_menu(plt);
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Edit")) {
      render_edit_menu();
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("View")) {
      render_view_menu();
      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }

private:
  std::unique_ptr<task_manager> m_task_manager{ task_manager::create() };

  app_state m_state{ "label_tool_project.yaml", m_task_manager.get() };

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
