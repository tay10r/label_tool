#include "help_widget.hpp"

#include <imgui.h>

namespace {

const char content[] = R"(Welcome!

To get started, navigate to the "View" menu in the main menu bar and select the widgets you'd like to make visible.
A good starting point is to make the image list widget and the image viewer widget visible.

This program has the concept of a "project". A project consists of data related to the creation of a new dataset.
A project uses an entire directory as a working space, however most of the data is stored in YAML file that has the
filename "label_tool_project.yaml". To create a new project, go to the "File" menu and hit "New Project". To open an
existing project, go to the "File" menu and hit "Open Project".

Feel free to close this widget. It can be reopened at any time by going to the "View" menu and checking the "Help" box.
)";

} // namespace

void
help_widget::render(edit_moderator<project>&)
{
  ImGui::TextUnformatted(content);
}
