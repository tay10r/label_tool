#include "background_model_widget.hpp"

namespace {

class background_model_widget_impl final : public background_model_widget
{
public:
  background_model_widget_impl(task_manager* task_mgr)
    : m_task_manager(task_mgr)
  {
  }

  void render(edit_moderator<project>& proj) override {}

  void on_project_modification(const project&) override {}

  void on_image_select(const project&, std::size_t index) override {}

private:
  task_manager* m_task_manager{ nullptr };
};

} // namespace

auto
background_model_widget::create(task_manager* task_mgr) -> std::unique_ptr<background_model_widget>
{
  return std::unique_ptr<background_model_widget>(new background_model_widget_impl(task_mgr));
}
