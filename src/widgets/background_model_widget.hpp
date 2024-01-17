#pragma once

#include "../widget.hpp"

#include <memory>

class task_manager;

class background_model_widget : public widget
{
public:
  static auto create(task_manager* task_mgr) -> std::unique_ptr<background_model_widget>;

  ~background_model_widget() override = default;
};
