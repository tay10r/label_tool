#pragma once

#include "../widget.hpp"

#include <limits>
#include <string>
#include <vector>

#include <cstddef>

class class_dist_widget final : public widget
{
public:
  void render(edit_moderator<project>& proj) override;

  void on_project_modification(const project& proj) override;

  void on_image_select(const project&, std::size_t) override {}

protected:
  void update_distribution(const project& proj);

private:
  bool m_dirty{ true };

  std::vector<std::string> m_names;

  std::vector<float> m_weights;
};
