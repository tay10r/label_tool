#include "image_list_widget.hpp"

#include "../edit_moderator.hpp"
#include "../project.hpp"

#include <algorithm>
#include <iterator>

#include <imgui.h>

image_list_widget::image_list_widget(void* cb_data, select_callback cb_func)
  : m_cb_data(cb_data)
  , m_cb_func(cb_func)
{
}

void
image_list_widget::render(edit_moderator<project>& proj)
{
  {
    int index = m_image_index;

    if (ImGui::SliderInt("Frame", &index, 0, proj.get().frames.empty() ? 0 : proj.get().frames.size())) {
      on_internal_image_select(static_cast<std::size_t>(index));
    }
  }

  ImGui::BeginDisabled(m_modify_state);
  if (ImGui::Button("Modify List")) {
    begin_modify_state(proj.get());
  }
  ImGui::EndDisabled();

  const auto image_selected = (m_image_index < proj.get().frames.size());

  ImGui::BeginDisabled(!m_modify_state);

  for (std::size_t i = 0; i < proj.get().class_defs.size(); i++) {
    ImGui::SameLine();

    ImGui::PushID(static_cast<int>(i));

    if (ImGui::Button(proj.get().class_defs.at(i).name.c_str())) {

      const int value = proj.get().class_defs.at(i).value;

      auto& ref = proj.edit();

      for (std::size_t i = 0; i < m_modify_selection_states.size(); i++) {
        ref.frames[i].class_id = value;
      }

      end_modify_state();
    }

    ImGui::PopID();
  }

  ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1, 0, 0, 1));
  if (ImGui::Button("Delete")) {
    auto& ref = proj.edit();
    std::vector<project::frame> result;
    result.reserve(ref.frames.size());
    for (std::size_t i = 0; i < ref.frames.size(); i++) {
      if (!m_modify_selection_states.at(i)) {
        result.emplace_back(std::move(ref.frames[i]));
      }
    }
    ref.frames = std::move(result);
    end_modify_state();
  }
  ImGui::PopStyleColor();

  ImGui::SameLine();

  if (ImGui::Button("Cancel")) {
    m_modify_state = false;
    m_modify_selection.clear();
    m_modify_selection_states.clear();
  }

  ImGui::EndDisabled();

  ImGui::Separator();

  if (!ImGui::BeginChild("Image List Table")) {
    return;
  }

  if (m_modify_state) {
    render_image_list_in_modify_state(proj);
  } else {
    render_image_list_without_modify_state(proj);
  }

  ImGui::EndChild();
}

void
image_list_widget::begin_modify_state(const project& proj)
{
  m_modify_state = true;
  m_modify_selection.clear();
  m_modify_selection_states.clear();
  m_modify_selection_states.resize(proj.frames.size(), false);
}

void
image_list_widget::end_modify_state()
{
  m_modify_state = false;
  m_modify_selection.clear();
  m_modify_selection_states.clear();
}

void
image_list_widget::render_image_list_in_modify_state(edit_moderator<project>& proj)
{
  const std::size_t num_frames = proj.get().frames.size();

  for (std::size_t i = 0; i < num_frames; i++) {

    std::string path = proj.get().frames[i].path.filename().string();

    auto s = !!m_modify_selection_states.at(i);

    if (!ImGui::Checkbox(path.c_str(), &s)) {
      continue;
    }

    m_modify_selection_states.at(i) = s ? 1 : 0;

    if (m_modify_selection.empty()) {
      m_modify_selection.emplace_back(i);
      continue;
    }

    if (ImGui::IsKeyDown(ImGuiKey_LeftShift) || ImGui::IsKeyDown(ImGuiKey_RightShift)) {

      const auto j_min = std::min(m_modify_selection.back(), i);
      const auto j_max = std::max(m_modify_selection.back(), i);

      for (std::size_t j = j_min; j <= j_max; j++) {
        m_modify_selection_states[j] = 1;
      }

      m_modify_selection.emplace_back(i);

    } else {

      m_modify_selection.emplace_back(i);
    }
  }
}

void
image_list_widget::render_image_list_without_modify_state(edit_moderator<project>& proj)
{
  const std::size_t num_frames = proj.get().frames.size();

  for (std::size_t i = 0; i < num_frames; i++) {

    const std::string path = proj.get().frames[i].path.filename().string();

    if (ImGui::Selectable(path.c_str(), m_image_index == i)) {
      on_internal_image_select(i);
    }
  }
}

void
image_list_widget::on_project_modification(const project& proj)
{
  if (m_modify_state) {
    m_modify_selection_states.clear();
    m_modify_selection.clear();
    m_modify_state = false;
  }
}

void
image_list_widget::on_image_select(const project& proj, std::size_t index)
{
  m_image_index = index;
}

void
image_list_widget::on_internal_image_select(std::size_t index)
{
  if (m_cb_func) {
    m_cb_func(m_cb_data, index);
  }
}
