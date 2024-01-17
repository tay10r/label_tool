#include "notes_widget.hpp"

#include "../edit_moderator.hpp"
#include "../project.hpp"

#include <imgui.h>

#include <imgui_stdlib.h>

void
notes_widget::render(edit_moderator<project>& proj)
{
  std::string notes = proj.get().notes;

  if (ImGui::InputTextMultiline("Notes", &notes)) {
    proj.edit().notes = std::move(notes);
  }
}
