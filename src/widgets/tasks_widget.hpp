#pragma once

#include "../widget.hpp"

class task_manager;

class tasks_widget final : public widget
{
public:
  tasks_widget(task_manager* task_mgr);

  void render(edit_moderator<project>&) override;

  void on_project_modification(const project&) override;

  void on_image_select(const project&, std::size_t image_index) override;

private:
  task_manager* m_task_manager{ nullptr };
};
