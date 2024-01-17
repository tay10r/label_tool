#include "widget_manager.hpp"

#include "edit_moderator.hpp"
#include "project.hpp"

#include <imgui.h>

void
widget_manager::add_widget(const char* name, std::unique_ptr<widget> w, bool visible)
{
  m_widgets.emplace(name, std::move(w));
}

void
widget_manager::render_visibility_menu(edit_moderator<project>& proj)
{
  std::size_t num_widgets = proj.get().widgets.size();

  for (std::size_t i = 0; i < num_widgets; i++) {

    bool visible = proj.get().widgets.at(i).visible;

    if (ImGui::Checkbox(proj.get().widgets.at(i).name.c_str(), &visible)) {
      proj.edit().widgets.at(i).visible = visible;
    }
  }
}

void
widget_manager::render_widgets(edit_moderator<project>& proj)
{
  std::size_t num_widgets = proj.get().widgets.size();

  for (std::size_t i = 0; i < num_widgets; i++) {

    bool visible = proj.get().widgets.at(i).visible;

    if (!visible) {
      continue;
    }

    auto& w = m_widgets.at(proj.get().widgets.at(i).name);

    if (ImGui::Begin(proj.get().widgets.at(i).name.c_str(), &visible)) {
      w->render(proj);
    }

    ImGui::End();

    if (!visible) {
      proj.edit().widgets.at(i).visible = false;
    }
  }
}

void
widget_manager::on_project_change(const project& proj)
{
  for (auto& w : m_widgets) {
    w.second->on_project_modification(proj);
  }
}

void
widget_manager::on_image_select(const project& proj, std::size_t image_index)
{
  for (auto& w : m_widgets) {
    w.second->on_image_select(proj, image_index);
  }
}
