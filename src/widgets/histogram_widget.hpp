#pragma once

#include "../widget.hpp"

#include <memory>

class histogram_widget : public widget
{
public:
  static auto create() -> std::unique_ptr<histogram_widget>;

  virtual ~histogram_widget() = default;
};
