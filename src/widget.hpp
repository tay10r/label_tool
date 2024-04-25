#pragma once

#include <cstddef>

struct project;

template<typename T>
class edit_moderator;

class widget
{
public:
  virtual ~widget() = default;

  virtual void render(edit_moderator<project>& proj) = 0;

  virtual void on_project_modification(const project& proj) = 0;

  virtual void on_image_select(const project& proj, std::size_t image_index) = 0;
};
