#pragma once

#include "widget.hpp"

#include <map>
#include <memory>
#include <string>

class widget_manager final
{
public:
  void add_widget(const char* name, std::unique_ptr<widget> w, bool visible);

  void render_visibility_menu(edit_moderator<project>& proj);

  void render_widgets(edit_moderator<project>& proj);

  void on_project_change(const project& proj);

  void on_image_select(const project& proj, std::size_t image_index);

private:
  std::map<std::string, std::unique_ptr<widget>> m_widgets;
};
