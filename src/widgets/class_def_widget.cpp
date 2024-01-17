#include "class_def_widget.hpp"

#include "../edit_moderator.hpp"
#include "../project.hpp"

#include <imgui.h>

#include <imgui_stdlib.h>

void
class_def_widget::render(edit_moderator<project>& proj)
{
  const std::size_t num_defs = proj.get().class_defs.size();

  for (std::size_t i = 0; i < num_defs; i++) {

    ImGui::PushID(static_cast<int>(i));

    std::string name = proj.get().class_defs[i].name;

    if (ImGui::InputText("Name", &name)) {
      proj.edit().class_defs[i].name = name;
    }

    int value = proj.get().class_defs[i].value;
    if (ImGui::InputInt("Value", &value)) {
      proj.edit().class_defs[i].value = value;
    }

    ImGui::Separator();

    ImGui::PopID();
  }

  if (ImGui::Button("Add")) {
    int value = 0;
    while (true) {
      auto good = true;
      for (const auto& def : proj.get().class_defs) {
        if (def.value == value) {
          good = false;
          break;
        }
      }
      if (good) {
        break;
      }
      value++;
    }
    proj.edit().class_defs.emplace_back(project::class_def{ "", value });
  }
}
