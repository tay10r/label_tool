#pragma once

#include "../widget.hpp"

class class_def_widget : public widget
{
public:
  void render(edit_moderator<project>& proj) override;

  void on_project_modification(const project&) override {}

  void on_image_select(const project&, std::size_t) override {}
};
