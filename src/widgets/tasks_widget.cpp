#include "tasks_widget.hpp"

#include "../task_manager.hpp"

#include <string>

#include <imgui.h>

tasks_widget::tasks_widget(task_manager* task_mgr)
  : m_task_manager(task_mgr)
{
}

void
tasks_widget::render(edit_moderator<project>&)
{
  auto active = m_task_manager->get_active();

  std::vector<std::pair<std::string, float>> active_tasks;

  for (const auto task_id : active) {

    const auto* name = m_task_manager->get_name(task_id);

    const auto completion_ratio = m_task_manager->get_completion_ratio(task_id);

    active_tasks.emplace_back(name, completion_ratio);
  }

  if (ImGui::BeginTable("Active Tasks", 2, ImGuiTableFlags_RowBg)) {

    for (const auto& t : active_tasks) {
      ImGui::TableNextRow();

      ImGui::TableNextColumn();
      ImGui::TextUnformatted(t.first.c_str());

      ImGui::TableNextColumn();
      ImGui::ProgressBar(t.second);
    }

    ImGui::EndTable();
  }

  // ImGui::SameLine();

  // ImGui::ProgressBar(completion_ratio);

  // auto queued = m_task_manager->get_queued();
  //
}

void
tasks_widget::on_project_modification(const project&)
{
}

void
tasks_widget::on_image_select(const project&, std::size_t)
{
}
